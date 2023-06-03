# quieter

## Quick Start 

```console
./build.sh
./quieter [datebook_example.txt]
```

## Why?

I am [Taskwarrior](https://en.wikipedia.org/wiki/Taskwarrior) enjoyer, but I find it difficult to use terminal commands in its workflow. I'd like to edit text files.

`quieter` is a tool that takes snapshot of your tasks stored in txt file (Check [./datebook_example.txt](./datebook_example.txt).) displays summary and exits.
There is no workflow, you just have to edit your text file

### Features of quieter

#### Subtasks

```
[ ] Zwiedzić Bałkany
    [ ] Zwiedzić Chorwacje
        [ ] Sprawdzić czy musze mieć paszport
        [ ] Tam jest euro czy kuny?
    [ ] Zwiedzić Bułgarię
```

Four-space indent indicates that this line is a subtask of the most recent line indented by `N-4` spaces

#### Priorities

```
[ ] Zwiedzić Bałkany
    [ ] Zwiedzić Chorwacje prio:M
        [ ] Sprawdzić czy musze mieć paszport
        [ ] Tam jest euro czy kuny?
    [ ] Zwiedzić Bułgarię prio:C
```

 - `prio:M` Must
 - `prio:S` Should
 - `prio:C` Could
 - `prio:W` Won't

each prio has its own coefficient

#### Due Dates

```
[ ] Zwiedzić Bałkany
    [ ] Zwiedzić Chorwacje prio:M due:2023-09-30
        [ ] Sprawdzić czy musze mieć paszport
        [ ] Tam jest euro czy kuny?
    [ ] Zwiedzić Bułgarię prio:C due:2026-01-01
```


### Autorefresh Trick

```
sudo apt install inotify-tools
```

```
while inotifywait -e modify ./datebook_example.txt; do clear; ./quieter ./datebook_example.txt; done
```


