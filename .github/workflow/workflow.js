var config = {
  template: "metro",
  orientation: "vertical-reverse",
  author: "Daniele E. Domenichelli <daniele.domenichelli@iit.it>",
};

var gitGraph = new GitGraph( config );
gitGraph.template.commit.message.displayHash = false;
gitGraph.template.commit.message.displayAuthor = false;
gitGraph.template.colors = ["#008fb5",
                            "#979797",
                            "#ae00e3",
                            "#f18f00"];

// Create branch named "master"
var master = gitGraph.orphanBranch({
    name: "master",
    column: 0
});

// Create branch named "devel"
var devel = gitGraph.orphanBranch({
    name: "devel",
    column: 2
});



devel.commit("Work in progress");

master.commit({ message: "YARP 2.3.64.5", tag: "v2.3.64.5" });
master.merge(devel, "Merge branch origin/master into devel");

devel.commit("Get ready for the new stable release");


devel.merge(master, "Merge branch origin/devel into master");
master.commit({ message: "YARP 2.3.66", tag: "v2.3.66" });
master.merge(devel, "Merge branch origin/master into devel");
devel.commit({ message: "Start YARP 2.3.68 development", tag: "(v2.3.67)" });


var feature_foo = devel.branch({ name: "feature_foo", column: 3 });
feature_foo.commit("Start working on feature 'foo'");
feature_foo.commit("Continue working on feature 'foo'");
feature_foo.commit("Finish implementing feature 'foo'");
feature_foo.merge(devel, "Merge branch feature_foo into devel");
devel.commit({ message: "Bump tweak number", tag: "(v2.3.67.1)" });


var bugfix_xxx = master.branch({ name: "bugfix_xxx", column: 1 });
bugfix_xxx.commit("Start working on issue #xxx");
bugfix_xxx.commit("Fix issue #xxx");
bugfix_xxx.merge(master, "Merge pull request #xxy from <user>/bugfix_xxx")
master.commit({ message: "YARP 2.3.66.1", tag: "v2.3.66.1" });
master.merge(devel, "Merge branch origin/master into devel")
