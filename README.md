> **Tip:** This file is written in Markdown. The raw view in the VS Code
> editor will show symbols such as `#`, `|`, and `` ``` `` instead of
> formatted headings, tables, and code blocks. For the formatted view,
> press **Ctrl+Shift+V** (macOS: **Cmd+Shift+V**) to open the rendered
> preview.

# Homework 4 — Parking Garage

## Overview

This program runs a small parking garage for one day. The garage has 3
car spaces, 2 motorcycle spaces, and 4 numbered bicycle racks. Every
vehicle gets a ticket number when it enters, and pays when it checks
out. A stay is billed by the *started* hour (1–60 minutes is 1 billed
hour, 61–120 minutes is 2, and so on), and all fees are computed in
whole cents:

| Vehicle | Rate per billed hour | Daily maximum | Extra rule |
|---|---|---|---|
| Car | $3.00 | $18.00 | Electric car: + $4.00 charging fee, not limited by the maximum |
| Motorcycle | $1.50 | $9.00 | — |
| Bicycle | free for 2 billed hours, then $0.50 | none | Locked to a numbered rack (1–4) |

The provided `main()` reads commands until `Q` or the end of the input:

| Command | Meaning |
|---|---|
| `C h:mm e` | park a car arriving at `h:mm` (`e` = 1 if electric, 0 if not) |
| `M h:mm` | park a motorcycle |
| `B h:mm` | park a bicycle at the lowest-numbered free rack |
| `X t h:mm` | check out the vehicle with ticket `t`, leaving at `h:mm` |
| `P` | list every vehicle that is still parked |
| `Q` | close the garage for the day |

Every check-out is added to the end of `parking_log.txt`.

`Vehicle` is the base class; `Car`, `Motorcycle`, and `Bicycle` are
derived from it. `main()`, the `ClockTime` structure, the global
constants, and the public sections of the four classes are provided and
complete. Your job is to declare the private members of the four classes
(TODO A through TODO D) and to write the twenty-four definitions marked
`TODO 1` through `TODO 24` in `main.cpp`. See the full assignment handout
on Canvas for the complete problem statement, the exact output format,
an example interaction, and the grading rubric.

## Constraints

- Do **not** modify `main()`, the provided constants, the `#include`
  lines, the `ClockTime` structure, the public section of any class, or
  any function's name, return type, or parameter list. The autograder
  calls your functions directly.
- Every member variable of the four classes must be private, and any
  helper member function you add must be private. Do not use the
  keywords `protected`, `friend`, or `virtual`.
- Use only the headers already included in the template (`<iostream>`,
  `<fstream>`, `<string>`, `<cstdlib>`). `std::vector`, pointers, and
  global variables (other than the provided constants) are not allowed.
  Use `std::string` only where the provided code already does. Compute
  every fee with `int` cents, not `double`.
- Use the provided named constants (`MINUTES_PER_HOUR`, `HOURS_PER_DAY`,
  `MAX_TICKET`, `BICYCLE_RACKS`, and the rate, maximum, and free-hour
  constants) instead of writing their values in your code.
- `Vehicle()` must delegate to `Vehicle(int, ClockTime)`, and
  `Car(int, ClockTime)` must delegate to `Car(int, ClockTime, bool)`.
  `Car(int, ClockTime, bool)`, `Motorcycle(int, ClockTime)`, and
  `Bicycle(int, ClockTime, int)` must pass `ticket` and `entry` to
  `Vehicle(int, ClockTime)` in their initialization sections.
  `Vehicle(int, ClockTime)` must call `isValidTime`, `checkOut(ClockTime)`
  must call `minutesBetween`, `checkOut(int, int)` must call
  `checkOut(ClockTime)`, and every `getFeeCents` must call `billedHours`.
- Your program must compile with no errors using
  `g++ -std=c++11 main.cpp -o main`.

## Function and Class Interface

**Free functions**

| Function | Parameters | Returns | Behavior |
|---|---|---|---|
| `makeTime` | `int hour`, `int minute` | `ClockTime` | Returns a `ClockTime` holding `hour` and `minute`, without checking them. |
| `isValidTime` | `ClockTime t` | `bool` | Returns `true` if the hour is 0–23 and the minute is 0–59. |
| `minutesBetween` | `ClockTime from`, `ClockTime to` | `int` | Returns the minutes from `from` to `to` on the same day (negative if `to` is earlier). |
| `billedHours` | `int minutes` | `int` | Returns the number of started hours in a stay of `minutes` minutes (0 for 0 or fewer minutes). |

**Class `Vehicle`**

| Member function | Parameters | Returns | Behavior |
|---|---|---|---|
| `Vehicle` | none | — | Creates a placeholder: ticket 0, entered at 0:00, still parked. |
| `Vehicle` | `int ticket`, `ClockTime entry` | — | Creates a parked vehicle, or prints `Invalid vehicle setup.` and ends the program with exit status 1 if the ticket or entry time is not valid. |
| `getTicket` | none | `int` | Returns the ticket number. |
| `isParked` | none | `bool` | Returns `true` until the first successful check-out. |
| `checkOut` | `ClockTime exitTime` | `int` | Checks the vehicle out and returns the minutes parked, or returns -1 and changes nothing if the check-out is not allowed. |
| `checkOut` | `int hour`, `int minute` | `int` | Same as `checkOut(makeTime(hour, minute))`. |
| `getMinutesParked` | none | `int` | Returns the minutes parked after check-out, or 0 while parked. |
| `output` | `std::ostream& outs` | `void` | Writes the one-line status in the handout's exact format to `outs`. |

**Class `Car : public Vehicle`** (also has every public member of `Vehicle`)

| Member function | Parameters | Returns | Behavior |
|---|---|---|---|
| `Car` | none | — | Creates a placeholder car that is not electric. |
| `Car` | `int ticket`, `ClockTime entry` | — | Creates a parked car that is not electric. |
| `Car` | `int ticket`, `ClockTime entry`, `bool electric` | — | Creates a parked car that is electric when `electric` is `true`. |
| `isElectric` | none | `bool` | Returns `true` for an electric car. |
| `getFeeCents` | none | `int` | Returns the car's fee in cents after check-out, or 0 while parked. |

**Class `Motorcycle : public Vehicle`**

| Member function | Parameters | Returns | Behavior |
|---|---|---|---|
| `Motorcycle` | none | — | Creates a placeholder motorcycle. |
| `Motorcycle` | `int ticket`, `ClockTime entry` | — | Creates a parked motorcycle. |
| `getFeeCents` | none | `int` | Returns the motorcycle's fee in cents after check-out, or 0 while parked. |

**Class `Bicycle : public Vehicle`**

| Member function | Parameters | Returns | Behavior |
|---|---|---|---|
| `Bicycle` | none | — | Creates a placeholder bicycle with rack number 0. |
| `Bicycle` | `int ticket`, `ClockTime entry`, `int rack` | — | Creates a parked bicycle at `rack`, or prints `Invalid vehicle setup.` and ends the program with exit status 1 if the arguments are not valid. |
| `getRack` | none | `int` | Returns the rack number. |
| `getFeeCents` | none | `int` | Returns the bicycle's fee in cents after check-out, or 0 while parked. |

## Compiling and Running

```
g++ -std=c++11 main.cpp -o main
./main            # macOS / Linux
.\main.exe        # Windows
```

The program reads from the keyboard and appends to `parking_log.txt` in
the folder where you run it. To repeat a test quickly, type the commands
into a text file (one per line) and redirect it into the program, for
example `./main < day1.txt` (macOS/Linux) or `.\main.exe < day1.txt`
(Windows Command Prompt; in PowerShell use
`Get-Content day1.txt | .\main.exe`).

## Submission Checklist

- [ ] Filled in the file header at the top of `main.cpp` (name, student ID, section).
- [ ] Declared all member variables of the four classes in their `private` sections.
- [ ] Program compiles with no errors using `g++ -std=c++11 main.cpp -o main`.
- [ ] Tested every command and fee rule, including stays of exactly 60 and 61 minutes, both daily maximums, an electric car, check-outs at and before the entry time, invalid times, a repeated check-out, full spaces, rack reuse, and a file without `Q`.
- [ ] Tested the constructors and functions that `main()` never calls (a plain `Vehicle`, `Car(ticket, entry)`, `isElectric`, `getMinutesParked`) with a separate driver program of my own, without changing the provided `main()` or submitting the driver.
- [ ] Code is commented and consistently formatted.
- [ ] Committed and pushed all changes to GitHub (not just once, right before the deadline).
- [ ] Submitted through Gradescope by selecting this repository and the correct branch.
- [ ] Submitted the Part A Concept Check answers as a PDF on Canvas.

## Academic Integrity

Write this code independently. Do not copy code from another student,
share your submission files with classmates, or use an AI tool to
generate, complete, modify, or correct any part of this program. See
the course syllabus, Section B, for the full policy. Keep this
repository private for the entire semester.
