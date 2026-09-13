# LICM
Loop Invariant Code Motion, which moves "Loop Invariant" code out side of loops to save on redundant calculations


## Quickstart
1. Install the [Nix: the packager manager](https://nixos.org/download/) with [Flakes](https://nixos.wiki/wiki/Flakes#Other_Distros,_with_Home-Manager) activated

2. Activate dev shell
```bash
$ nix develop
```

3. Build
```bash
$ cmake -S . -B build -G Ninja
```
4. Test
```bash
$ (cd build/ && ninja test)
```
or

```bash
$ (cd build/ && ninja test > build.log 2>&1)
```
