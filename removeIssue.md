# `remove_Task` vs `initd` retiring itself

## What we saw

After `initd` started writing **HELLO** on the 7-segment display, we added the script’s “remove myself from the scheduler” step:

```c
// template/src/main.c  (initd — these two lines are correct, keep them)
add_Task(&cmdShell, 0, "cmdShell", 99);

uint32_t my_id = CurrentTCB->id;
remove_Task(my_id);

while (1) {}
```

Symptoms:

- Display showed HELLO and then froze there.
- Serial never printed `shell #`.
- If we **commented out** `CurrentTCB->id` / `remove_Task(my_id)`, the shell came back and we could start `count` / `flash` over HELLO.

So the bug was not HELLO, not the shell task itself, and not those two lines in `initd`. It was **`remove_Task` changing `CurrentTCB` while the CPU was still inside `initd`**.

---

## How a context switch actually works

From `scheduler.c` comments (this is the supplied `context_switch.c.obj`, not our code):

1. SysTick quantum fires (every 100 µs).
2. **Save** the live CPU registers onto the stack of **whoever `CurrentTCB` points at**.
3. Call `scheduler()` to pick the next TCB and assign `CurrentTCB`.
4. **Load** registers from the new `CurrentTCB`.

`remove_Task` is **not** a context switch. It only edits the linked list. The function that called it keeps running on the same stack until the next tick.

Each task’s stack lives **inside** its TCB:

```c
struct tcb {
    uint32_t *Stack_Pointer;
    uint32_t  id;
    uint8_t   priority;
    char      name[32];
    uint32_t  TCB_Stack[STACK_SIZE];  // this IS the stack
    struct tcb *next;
};
```

So `CurrentTCB` must still describe **the task that is actually running** until the switcher has saved that task. If you point `CurrentTCB` at someone else first, the next save writes the running task’s registers **over the other task’s stack**.

---

## The broken code

After a correct unlink, the old `remove_Task` did this:

```c
// template/src/remove_task.c  — THE BUG (do not put this back)

int was_current = (CurrentTCB == curr); // is the task we just unlinked running?

if (was_current)
{
    if (curr->next != NULL) CurrentTCB = curr->next;
    else CurrentTCB = Task_List;
}

ENABLE_INT();

if (!was_current) free(curr);
```

Unlink and “don’t `free` the running TCB” were already right. The `if (was_current) { CurrentTCB = ... }` block was the error.

The idea was: “if we just removed the running task, immediately make `CurrentTCB` the next one so the scheduler doesn’t follow a dead node.” That sounds like round-robin, but it fights the switcher, which **saves first, then calls `scheduler()`**.

---

## What happened on the wire, tick by tick

Just before `remove_Task(my_id)`:

```text
Task_List:   initd  →  cmdShell  →  NULL
CurrentTCB:  initd          ← CPU is here, about to retire
```

`initd` is the list head, so unlink does `Task_List = curr->next`:

```text
Task_List:   cmdShell  →  NULL
initd->next: still cmdShell   (the zombie node is off the list, but not freed)
CurrentTCB:  still initd      so far so good
```

Then the bad block ran because `was_current` was true:

```text
CurrentTCB = curr->next = cmdShell
```

CPU is **still executing `initd`** (`while (1) {}`), but `CurrentTCB` now names **cmdShell**.

Next SysTick:

1. Save live registers (PC in `initd`’s spin loop, SP = `initd`’s stack) **into `cmdShell`’s TCB**. That overwrites the dummy frame `add_Task` planted for the shell (PSR, PC = `&cmdShell`, R0, SP at R8).
2. `scheduler()` does `t = CurrentTCB->next`. `cmdShell->next` is `NULL`, so it wraps to `Task_List` (`cmdShell` again). `CurrentTCB` stays `cmdShell`.
3. Load that smashed stack. The kernel “resumes” `initd`’s spin using the shell’s TCB.

`cmdShell` never starts. HAL on the other core keeps drawing `display_buffer`, so HELLO stays up. No `shell #`.

That is why commenting out `remove_Task(my_id)` “fixed” the shell: `initd` stayed on the list, `CurrentTCB` was never pointed at `cmdShell` early, and the dummy shell frame survived.

---

## Why stock `always free()` is also wrong here

The recovered stock `remove_Task` never touched `CurrentTCB`. It unlinked and **always** `free(curr)`.

That is safe when **the shell** does `rt <id>` on some *other* task (you are not running on that TCB).

It is not safe when **`initd` deletes itself**. `free(curr)` frees the whole TCB, including `TCB_Stack[]` — the stack the CPU is still using. That is use-after-free, worse than the `CurrentTCB` retarget.

Stock never hit this because the template `initd` never retired; it sat in `while (1) {}` forever. The lab script *does* require self-remove, so we cannot copy stock blindly.

---

## The fix

Leave `CurrentTCB` alone. Unlink only. Leak the running TCB on purpose (never `free` it). Capture `is_current` **before** `ENABLE_INT()`.

```c
// template/src/remove_task.c  — current (fixed)

    if (prev == NULL)
        Task_List = curr->next;
    else
        prev->next = curr->next;

    int is_current = (CurrentTCB == curr);

    ENABLE_INT();

    if (!is_current)
        free(curr);

    return (int)Task_ID;
```

After `initd` calls this:

```text
Task_List:   cmdShell  →  NULL
CurrentTCB:  initd          (zombie, still allocated, CPU still here)
initd->next: cmdShell
```

`initd` spins for at most one quantum. Next `scheduler()`:

```c
t = CurrentTCB;        // initd
t = t->next;           // cmdShell
CurrentTCB = t;        // shell — dummy frame still intact
```

Save happened **on `initd`’s stack**, which we never freed. Load happens from `cmdShell`. Shell prints `shell #`. `pt` no longer lists `initd` because it is not on `Task_List`.

Missing id still returns `0` (script), not `-1`.

---

## What not to “fix”

| Change | Verdict |
|--------|---------|
| Delete `remove_Task(my_id)` in `initd` | No — that is the required retire. It only looked like the bug. |
| `CurrentTCB = next` inside `remove_Task` | No — save would hit the wrong TCB. |
| `free()` the running TCB | No — that *is* the stack. |
| `return` from `initd` after remove | No — dummy LR is 0; you must `while (1) {}` until the tick. |

`rt` of a **non-running** task still `free()`s as before. Only self-remove (or `rt` of the task that is currently on the CPU) leaks one TCB until reset.

---

## How we knew it was fixed

1. Boot: HELLO on the left of the display.
2. Serial: `Starting HAL` then `shell #`.
3. `pt`: `cmdShell` only (no `initd`).
4. Starting `count` / `flash` overwrites HELLO on those digits. No extra “clear Hello” step is required.

---

## Why the recovered stock `remove_Task` would not have solved it

This is the body recovered from `template/obj/remove_task.c.obj`:

```c
int remove_Task(uint32_t Task_ID)
{
    TCB_t *prev = NULL;
    TCB_t *curr;

    curr = Task_List;
    DISABLE_INT();

    while (curr != NULL) {
        if (curr->id == Task_ID)
            break;
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) {
        ENABLE_INT();
        return -1;
    }

    if (prev == NULL)
        Task_List = curr->next;
    else
        prev->next = curr->next;

    free(curr);
    ENABLE_INT();
    return (int)Task_ID;
}
```

Stock **does not** assign `CurrentTCB`. That part is better than our old `was_current` block. Unlink-by-id is the same idea as ours.

It still would not have made `initd` retire safely, because of this line:

```c
free(curr);
```

There is no `if (CurrentTCB != curr)` guard. When `initd` does `remove_Task(my_id)`, `curr` **is** the TCB the CPU is executing on. That struct contains `TCB_Stack[]` — the live stack.

After `free(curr)`:

- `initd` is still inside `remove_Task` / `while (1)`, using memory that is now on the heap.
- `CurrentTCB` still points at that freed node.
- The next `scheduler()` does `CurrentTCB->next` — a dangling pointer.
- The next SysTick **saves** registers onto a stack that `malloc` may already have reused.

That is use-after-free, not a clean switch to `cmdShell`. HELLO can still sit on the glass (HAL is on the other core). `shell #` is not reliable; you can hang or hard-fault depending on what the heap does next.

Stock got away with `always free()` because the **template** `initd` never called `remove_Task` on itself. It looped forever. The only stock path was `rt` from the **shell**, deleting some *other* task — then `free` is safe. The lab script *does* require self-remove, so we cannot drop this object in as-is.

Return `-1` on a missing id also disagrees with Appendix A (`0`). That is a spec mismatch, not the crash.

| | Stock `.obj` | Old student code | Fix |
|--|--|--|--|
| Unlink by id | yes | yes | yes |
| Move `CurrentTCB` | no | **yes — smashed the shell** | no |
| `free` a task `rt` killed | yes | yes | yes |
| `free` the **running** task | **yes — frees the live stack** | no | no (leak that one TCB) |
| Missing id | `-1` | `0` | `0` |

The working function is stock’s “don’t touch `CurrentTCB`” **and** our “don’t `free` if `CurrentTCB == curr`.” Either piece alone is not enough for `initd` to retire.
