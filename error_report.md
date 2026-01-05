# Error Report

1. **Step 67** -> `git push` failed with 128 -> `git config` should not have `url.insteadof` set.
    - **Why Sloppy**: Assumed default environment was clean.
    - **What Missed**: Existence of global git config rewriting HTTPS to SSH.
    - **How Prevent**: Run `git config --list` before any git operations.

2. **Step 81** -> Deployment stuck on "Hello World" -> `git push origin main:master` should be used.
    - **Why Sloppy**: Forgot GitHub Pages default branch behavior.
    - **What Missed**: Legacy `master` branch on remote taking precedence over `main`.
    - **How Prevent**: Check `gh repo view` or remote branches before first push.

3. **Step 210** -> `make` built `demo_01` instead of `buddy_fragment` -> `Cwd` path should be valid.
    - **Why Sloppy**: Failed to verify `cp` and `mkdir` results, created double nesting `kernel_exercises/kernel_exercises`.
    - **What Missed**: `mv` command into existing directory creates subdirectory.
    - **How Prevent**: Always `ls -F` after bulk move operations.
