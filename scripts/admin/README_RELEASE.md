Steps for building a YARP stable release
========================================

* Make sure that the `master` and `devel` branches are up to date and that
  there are no local modifications or uncommitted files.
  Checkout the `master` branch.

```
  git fetch origin --prune
  git checkout -f devel
  git reset --hard origin/devel
  git checkout -f master
  git reset --hard origin/master
  git status
```

* Check git tags to get the current version, say `v3.1.1`.
  Next stable release will be `v3.1.2`.
  Let's release `v3.1.2`.

* Check regression tests


* Bump the fourth number in the `project` command in the main `CMakeLists.txt`.
  Ensure that the resulting version is the same as the one that we are going to
  create.

* Ensure that the release file for the release exists in `doc/release/`
  (`doc/release/v3_1_2.md` for `v3.1.2`).
  * Update the date for the release in this file.
  * Update the list of contributors by running

```
  echo >> doc/release/v3_1_1.md
  echo "\`\`\`" >> doc/release/v3_1_1.md
  git shortlog -ens --no-merges v3.1.0..master >> doc/release/v3_1_1.md
  echo "\`\`\`" >> doc/release/v3_1_1.md
```

* Ensure that the release file is linked in `doc/releases.dox`

* Update the file `doc/installation/download.dox`

* Commit:

```
git add CMakeLists.txt
git add doc/release/v3_1_2.md
git add doc/releases.dox
git add doc/installation/download.dox
git commit -m "YARP 3.1.2"
```

* Tag:

```
  git tag -a -m "YARP 3.1.2" v3.1.2 HEAD
```

* Further checks before pushing:

```
  git log --oneline --graph --decorate origin/master...HEAD
```

  (Check that the only commit is something like
  `* xxxxxxx (tag: v3.1.2, HEAD -> master) YARP 3.1.2`)


* (Skip this and the next steps if this is the latest release for this series)
  Prepare for the next stable release by adding the relative file in
  `doc/release/` (in this example `v3_1_3.md`) and add the releative file in
  `doc/releases.dox` in the same "Series" above the one for the file just
  released.

* Commit

```
git add doc/release/v3_1_3.md
git add doc/releases.dox
git commit -m "Prepare for next stable release (3.1.3)"
```

* Merge the changes into the `devel` branch

```
  git checkout devel
  git merge --no-ff -s recursive -X ours master
```

* Open https://github.com/robotology/yarp/settings/branch_protection_rules/1548467,
  remove the tick from `Include administrators` and save changes.

* Push with:

```
  git push origin master
  git push origin v3.1.2
  git push origin devel
```

* Open https://github.com/robotology/yarp/settings/branch_protection_rules/1548467,
  restore the tick in `Include administrators` and save changes.

* Edit the following labels on github
  (in https://github.com/robotology/yarp/labels)
  * For the current release:
    * Rename `Fixed in: master` to `Fixed in: v3.1.2`
    * Rename `Target: master` to `Target: v3.1.2`
    * Create `Affects: YARP v3.1.2` using `#006b75` as color.
  * For the next stable release (if any):
    * Create `Fixed in: YARP master` using `#fbca04` as color.
    * Create `Target: YARP master` using `#d93f0b` as color.

* Follow the steps in the "Actions after any YARP release" section


Steps for building a YARP feature release
=========================================

* Make sure that the `master` and `devel` branches are up to date and that
  there are no local modifications or uncommitted files.
  Checkout the `master` branch.

```
  git fetch origin --prune
  git checkout -f devel
  git reset --hard origin/devel
  git checkout -f master
  git reset --hard origin/master
  git status
```

* Merge `devel` into master

```
  git merge --no-ff devel
```

* Check git tags to get the current version, say `v3.1.0`.
  Next feature release will be `v3.2.0`.
  Let's release `v3.2.0`.

* Check regression tests

* Bump the third number and make sure that there is not a fourth number in the
  `project` command in the main `CMakeLists.txt`.
  Ensure that the resulting version is the same as the one that we are going to
  create.

* Prepare licenses

```
  rm -rf license_check
  ./scripts/admin/update-license
  cp license-statement.txt COPYING
  # manually fix COPYING file
```

* Ensure that the release file for the release exists in `doc/release/`
  (`doc/release/v3_2_0.md` for `v3.2.0`).
  * Update the date for the release in this file.
  * Update the list of contributors by running

```
  git shortlog -ens --no-merges v3.1.0..master
```

* Ensure that the release file is linked in `doc/releases.dox`

* Update the file `doc/installation/download.dox`

* Commit:

```
  git commit -a -m "YARP 3.2.0"
```

* Tag:

```
  git tag -a -m "YARP 3.2.0" v3.2.0 HEAD
```

* Further checks before pushing:

```
  git log --oneline --graph --decorate origin/master...HEAD
```

  (Check that the last commit is something like
  `* xxxxxxx (tag: v3.2.0, HEAD -> master) YARP 3.2.0` and that the previous
  commit is something like `*   xxxxxxx Merge branch 'devel'`)

* Prepare for the next stable release by adding the relative file in
  `doc/release/` (in this example `v3_2_1.md`) and add the releative file in
  `doc/releases.dox` in the same "Series" above the one for the file just
  released.

* Commit

```
git add doc/release/v3_2_1.md
git add doc/releases.dox
git commit -m "Prepare for next stable release (3.2.1)"
```

* Merge the changes into the `devel` branch

```
  git checkout devel
  git merge --no-ff master
```

* Bump the third number in the `project` command in the main `CMakeLists.txt` by
  one (it should be an odd number, in this example `71`).

* Prepare for the next feature release by adding the relative file in
 `doc/release/` (in this example `v3_3_0.md`)
  and add the releative file in `doc/releases.dox` in a new same "Series" on top.

* Commit

```
git add CMakeLists.txt
git add doc/release/v3_3_0.md
git add doc/releases.dox
git commit -m "Prepare for next feature release (3.3.0)"
```

* Open https://github.com/robotology/yarp/settings/branches/master, remove tick
  from `Include administrators` and save changes.

* Push with:

```
  git push origin master
  git push origin v3.2.0
  git push origin devel
```

* Open https://github.com/robotology/yarp/settings/branches/master, restore tick
  in `Include administrators` and save changes.

* Create the following labels on github
  (in https://github.com/robotology/yarp/labels)
  * For the current release:
    * `Affects: YARP v3.2.0` using `#006b75` as color.
  * For the next stable release:
    * `Fixed in: YARP 3.2.1` using `#fbca04` as color.
    * `Target: YARP v3.2.1` using `#d93f0b` as color.
  * For the next feature release:
    * `Fixed in: YARP 3.3.0` using `#fbca04` as color.
    * `Target: YARP v3.3.0` using `#d93f0b` as color.



Actions after any YARP release
==============================

* Create the release on github at
  https://github.com/robotology/yarp/releases/new, using the newly created tag,
  title "YARP x.y.z (YYYY-MM-DD)", and copying the release notes file as the
  release description (removing the first 5 lines).

* Create and upload the binary packages.

* Create an issue in https://github.com/robotology/homebrew-formulae/issues
  titled ** Update YARP formula to [versionnum] **

* Publish the release by posting an announcement in
  [`robotology/QA`](https://github.com/robotology/QA/issues/new).

* Update the latest release version in
  [YARP's Wikipedia page](https://en.wikipedia.org/wiki/YARP).

