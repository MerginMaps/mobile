
# How we work

...and how we expect you to work in this repository 🤓.

This document is useful for internal contributors; some sections (for example, PULL REQUESTS and BRANCH RULES) are useful for external contributors as well.

## Git

We follow a simple trunk-based workflow. The `master` branch contains the latest code; feature and bugfix branches are merged into `master` once reviewed and tested.

For hotfix releases, checkout the most recent tag, create a new `hotfix/` branch, and release from there.

Task state is tracked on the [project board](https://github.com/orgs/MerginMaps/projects/15). Developers are responsible for keeping their tasks in the correct state.

### Branch rules

We use the `feature/`, `bugfix/`, and `hotfix/` prefixes as appropriate.

Branches should be deleted once merged, except for `hotfix/` branches, which must be retained.

Force-pushes are not allowed on `master`. All changes must come via pull requests.

## Pull requests (PR)

Pull requests should include a clear, concise description of the changes and reference the issue they address (see [GitHub docs](https://docs.github.com/en/get-started/writing-on-github/working-with-advanced-formatting/using-keywords-in-issues-and-pull-requests#linking-a-pull-request-to-an-issue) for linking issues). In exceptional cases (for example, CI updates or version bumps) a linked issue is not required, but the PR description must explain the change.

If you update the app UI or UX, include images or a short video—preferably showing the before and after.

C++ changes should include unit tests where applicable.

Open draft PRs when you have an initial concept and want feedback. Do not open drafts for very early commits that would flood the PR page.

## Release management

Issues are grouped via GitHub milestones. Each milestone has an associated issue in the releases repository: https://github.com/MerginMaps/releases/issues.

When a release is ready for testing, include information about where to find builds in the associated ticket.

The QA team should maintain a regression testing report on the release ticket. Each discovered bug should have a new GitHub issue created.

Create a GitHub release and a git tag for each production release.

Release procedure: https://docs.google.com/document/d/1bpJZ21yx_yQKGBWUVUl32dSBR97qfvUHm8dCMilEUeA/edit?usp=sharing

### Hotfix releases

Hotfix releases use branch names like `hotfix/major_minor_patch-release-keywords` and are never deleted.

Git tags are generated on these branches.

If appropriate, merge hotfix changes back into `master`.

## Project board

Project board states:
 - `Backlog` — not in the development queue yet
 - `Todo` — ready for work, waiting for its turn
 - `In Progress` — someone is actively working on the task
 - `In Review` — a pull request is open (not draft) and awaiting review or addressing comments
 - `Testing` — QA should test the changes; status can be waiting, passed, or failed
 - `Merged` — merged into `master`
 - `Completed` — published

If a task fails testing, QA should label the issue (or PR if there is no issue) with `testing-failed`. The task developer should then communicate with the tester, move the task back to `In Progress`, and remove the `testing-failed` label.

Tasks in `Testing` without `testing-failed` or `testing-ok` labels are expected to be tested.

Duplicate tasks should be archived from the board.

Once a PR is accepted by a reviewer, the developer should move the task to `Testing` and add links to builds (iOS, Android, Windows) as a comment in the issue or PR.


... now let's get to work 💪
