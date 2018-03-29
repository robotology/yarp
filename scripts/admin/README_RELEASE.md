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

* Check git tags to get the current version, say `v2.3.70.1`.
  Next stable release will be `v2.3.70.2`.
  Let's release `v2.3.70.2`.

* Check regression tests


* Bump the fourth number in the `project` command in the main `CMakeLists.txt`.
  Ensure that the resulting version is the same as the one that we are going to
  create.

* Ensure that the release file for the release exists in `doc/release/`
  (`doc/release/v2_3_70_2.md` for `v2.3.70.2`).
  * Update the date for the release in this file.
  * Update the list of contributors by running

```
  git shortlog -ens --no-merges v2.3.70.1..master
```

* Ensure that the release file is linked in `doc/releases.dox`

* Update the file `doc/installation/download.dox`

* Commit:

```
git add CMakeLists.txt
git add doc/release/v2_3_70_2.md
git add doc/releases.dox
git add doc/installation/download.dox
git commit -m "YARP 2.3.70.2"
```

* Tag:

```
  git tag -a -m "YARP 2.3.70.2" v2.3.70.2 HEAD
```

* Further checks before pushing:

```
  git log --oneline --graph --decorate origin/master...HEAD
```

  (Check that the only commit is something like
  `* xxxxxxx (tag: v2.3.70.2, HEAD -> master) YARP 2.3.70.2`)


* (Skip this and the next steps if this is the latest release for this series)
  Prepare for the next stable release by adding the relative file in
  `doc/release/` (in this example `v2_3_70_3.md`) and add the releative file in
  `doc/releases.dox` in the same "Series" above the one for the file just
  released.

* Commit

```
git add doc/release/v2_3_70_3.md
git add doc/releases.dox
git commit -m "Prepare for next stable release (2.3.70.3)"
```

* Merge the changes into the `devel` branch

```
  git checkout devel
  git merge --no-ff master
```

* Open https://github.com/robotology/yarp/settings/branches/master, remove tick
  from `Include administrators` and save changes.

* Push with:

```
  git push origin master
  git push origin v2.3.70.2
  git push origin devel
```

* Open https://github.com/robotology/yarp/settings/branches/master, restore tick
  in `Include administrators` and save changes.

* Create the following labels on github
  (in https://github.com/robotology/yarp/labels)
  * For the current release:
    * `Affects: YARP v2.3.70.2` using `#006b75` as color.
  * For the next stable release (if any):
    * `Fixed in: YARP v2.3.70.3` using `#fbca04` as color.
    * `Target: YARP v2.3.70.3` using `#d93f0b` as color.


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

* Check git tags to get the current version, say `v2.3.70`.
  Next feature release will be `v2.3.72`.
  Let's release `v2.3.72`.

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
  (`doc/release/v2_3_72.md` for `v2.3.72`).
  * Update the date for the release in this file.
  * Update the list of contributors by running

```
  git shortlog -ens --no-merges v2.3.70..master
```

* Ensure that the release file is linked in `doc/releases.dox`

* Update the file `doc/installation/download.dox`

* Commit:

```
  git commit -a -m "YARP 2.3.72"
```

* Tag:

```
  git tag -a -m "YARP 2.3.72" v2.3.72 HEAD
```

* Further checks before pushing:

```
  git log --oneline --graph --decorate origin/master...HEAD
```

  (Check that the last commit is something like
  `* xxxxxxx (tag: v2.3.72, HEAD -> master) YARP 2.3.72` and that the previous
  commit is something like `*   xxxxxxx Merge branch 'devel'`)

* Prepare for the next stable release by adding the relative file in
  `doc/release/` (in this example `v2_3_72_1.md`) and add the releative file in
  `doc/releases.dox` in the same "Series" above the one for the file just
  released.

* Commit

```
git add doc/release/v2_3_72_1.md
git add doc/releases.dox
git commit -m "Prepare for next stable release (2.3.72.1)"
```

* Merge the changes into the `devel` branch

```
  git checkout devel
  git merge --no-ff master
```

* Bump the third number in the `project` command in the main `CMakeLists.txt` by
  one (it should be an odd number, in this example `71`).

* Prepare for the next feature release by adding the relative file in
 `doc/release/` (in this example `v2_3_74.md`)
  and add the releative file in `doc/releases.dox` in a new same "Series" on top.

* Commit

```
git add CMakeLists.txt
git add doc/release/v2_3_74.md
git add doc/releases.dox
git commit -m "Prepare for next feature release (2.3.74)"
```

* Open https://github.com/robotology/yarp/settings/branches/master, remove tick
  from `Include administrators` and save changes.

* Push with:

```
  git push origin master
  git push origin v2.3.72
  git push origin devel
```

* Open https://github.com/robotology/yarp/settings/branches/master, restore tick
  in `Include administrators` and save changes.

* Create the following labels on github
  (in https://github.com/robotology/yarp/labels)
  * For the current release:
    * `Affects: YARP v2.3.72` using `#006b75` as color.
  * For the next stable release:
    * `Fixed in: YARP 2.3.72.1` using `#fbca04` as color.
    * `Target: YARP v2.3.72.1` using `#d93f0b` as color.
  * For the next feature release:
    * `Fixed in: YARP 2.3.74` using `#fbca04` as color.
    * `Target: YARP v2.3.74` using `#d93f0b` as color.



Actions after any YARP release
==============================

* Create the release on github, copying the release notes file as the release
  description and save as draft.

* Create and upload the binary packages.

* Create an issue in https://github.com/robotology/homebrew-formulae/issues
  titled ** Update YARP formula to [versionnum] **

* Publish the release by posting an announcement in
  [`robotology/QA`](https://github.com/robotology/QA) and in the
  [`rc-hackers`](http://wiki.icub.org/wiki/Robotcub-hackers) mailing list.

* Update the latest release version in
  [YARP's Wikipedia page](https://en.wikipedia.org/wiki/YARP).

