Steps for building a YARP release
=================================

* Checkout latest `devel` branch and make sure that there are no local modifications or uncommited files:

```
  git checkout -f devel
  git fetch origin
  git reset --hard origin/devel
  git status 
```

* Checkout latest `master` and make sure that there are no local modifications or uncommited files:

```
  git checkout -f master
  git fetch origin
  git reset --hard origin/master
  git status 
```

* Merge `devel` into master

```
  git merge --no-ff devel
```

* Check git tags to get the current version, say `v2.3.66`.
  Next release stable release will be `v2.3.66.1`, next feature release
  will be `v2.3.68`.
  Let's release `v2.3.68`.

* Check regression tests

* Make sure that `conf/YarpVersion.cmake` is different to the latest
  public version, bumping up the version number if needed.

  Bump `YARP_VERSION_PATCH` and make `YARP_VERSION_TWEAK` to empty for
  feature releases.
  Bump `YARP_VERSION_TWEAK` for stable releases.
  
  Ensure that the version should is the same of the tag that we are
  going to create.

* Prepare licenses

```
  rm -rf license_check
  ./scripts/admin/update-license
  # check license-statement.txt for sanity
  cp license-statement.txt COPYING
```

* Ensure that the release file for the release exists in `doc/release/`
  (for example `v2_3_68.md` for `v2.3.68`, `v2_3_66_1` for `v2.3.66.1`.
  Update the date for the release in this file.
  Update the list of contributors by running
  
```  
  git shortlog -ens --no-merges v2.3.66..master
```

* Ensure that the release file is linked in `doc/releases.dox`

* Update the file `doc/installation/installation_downloads.dox`

* Commit:

```
  git commit -a -m "YARP 2.3.68"
```

* Tag:

```
  git tag -a -m "YARP 2.3.68" v2.3.68 HEAD
```

* Further checks before pushing:

```
  git log --oneline --graph --decorate origin/master...HEAD
```

 (Check that the only commit is like `* xxxxxxx (tag: v2.3.68, HEAD, master) YARP 2.3.68`)


* Prepare for the next stable release by adding the relative file in `doc/release/` (in this example `v2_3_68_1.md)
  and add the releative file in `doc/releases.dox` in the same "Series" above the one for the file just released.

* Commit

```
git add doc/release/v2_3_68_1.md
git add doc/releases.dox
git commit -m "Prepare for next stable release (2.3.68.1)"
```

* Merge the changes into the `devel` branch

```
  git checkout -f devel
  git fetch origin
  git reset --hard origin/devel
  git merge --no-ff master
```

* Bump `YARP_VERSION_PATCH` in `conf/YarpVersion.cmake` by one (it should be an odd number, in this example `69`).

* Prepare for the next feature release by adding the relative file in `doc/release/` (in this example `v2_3_70.md)
  and add the releative file in `doc/releases.dox` in a new same "Series" on top.

* Commit

```
git add conf/YarpVersion.cmake
git add doc/release/v2_3_70.md
git add doc/releases.dox
git commit -m "Prepare for next feature release (2.3.70)"
```

* Create a label on github (in https://github.com/robotology/yarp/labels) for bug fixed
  in next stable release (`Fixed in: YARP 2.3.68.1`) and one for bug fixed in next feature release (`Fixed in: YARP 2.3.70`). Use `#fbca04` as color.

* Open https://github.com/robotology/yarp/settings/branches/master, remove tick from `Include administrators` and save changes.

* Push with:

```
  git push origin master
  git push origin v2.3.68
  git push origin devel
```

* Open https://github.com/robotology/yarp/settings/branches/master, restore tick in `Include administrators` and save changes.

* Create the release on github, copying the release notes file as the release description and save as draft.

* Create and upload the binary packages.

* Publish the release.
