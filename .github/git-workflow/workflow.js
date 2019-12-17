const branchesOrder = ["security_fix_on_oldstable",
                       "yarp-3.3",
                       "bugfix_on_stable",
                       "yarp-3.4",
                       "new_feature_on_master",
                       "master"];

var compareBranchesOrder = function(a, b) { return branchesOrder.indexOf(a) - branchesOrder.indexOf(b); };

const options = {
  template: GitgraphJS.templateExtend("metro", {
    colors: ["#008fb5",
             "#979797",
             "#ae00e3",
             "#123456",
             "#f18f00",
             "#00aa00"],
    commit: {
      message: {
        displayAuthor: false,
        displayHash: false,
        font: '11pt monospace',
      },
      spacing: '32',
      dot: {
        size: '8'
      }
    },
    branch: {
      lineWidth: '7',
      label: {
        font: 'italic 9pt serif',
      },
    },
    tag: {
      font: 'bold 11pt monospace',
    }
  }),
  author: "Daniele E. Domenichelli <daniele.domenichelli@iit.it>",
  compareBranchesOrder: compareBranchesOrder
};


const graphContainer = document.getElementById("gitgraph");
const gitgraph = GitgraphJS.createGitgraph(graphContainer, options);

// Create branch named "master"
var master = gitgraph.branch("master");
// Add some spacing
master.commit({ subject: "", style: { dot: { size: '0' }} });
master.commit({ subject: "YARP 3.3.0", tag: "v3.3.0" });


// Create branch named "yarp-3.3"
var oldstable = master.branch("yarp-3.3");
oldstable.commit("Start YARP 3.3.1 development");
master.merge(oldstable, "Merge branch yarp-3.3 into master");
master.commit("Start YARP 3.4 development").tag({ name: "(v3.3.100)", style: { color: "#00aa0060", bgColor: "#fff"} });

// Add some spacing
oldstable.commit({ subject: "", style: { dot: { size: '0' }} });
oldstable.commit({ subject: "", style: { dot: { size: '0' }} });


oldstable.commit({ subject: "YARP 3.3.1", tag: "v3.3.1" });
master.merge(oldstable, "Merge branch yarp-3.3 into master");

master.commit({ subject: "", style: { dot: { size: '0' }} });
master.commit({ subject: "YARP 3.4.0", tag: "v3.4.0" });

// Create branch named "yarp-3.4" and start new development on master branch
var stable = master.branch("yarp-3.4");
master.commit("Start YARP 3.5 development").tag({ name: "(v3.4.100)", style: { color: "#00aa0060", bgColor: "#fff"} });
master.commit({ subject: "", style: { dot: { size: '0' }} });


// Create a security fix on oldstable branch
oldstable.commit("Start YARP 3.3.2 development");
var security_fix_on_oldstable = oldstable.branch({ name: "security_fix_on_oldstable", style: { lineWidth: '4' }});
security_fix_on_oldstable.commit("Start working on issue #xxx");
security_fix_on_oldstable.commit("Fix issue #xxx");
oldstable.merge(security_fix_on_oldstable, "Merge pull request #xxy from <user>/security_fix_on_oldstable")
stable.merge(oldstable, "Merge branch yarp-3.3 into yarp-3.4");
master.merge(stable, "Merge branch yarp-3.4 into master");

master.commit({ subject: "", style: { dot: { size: '0' }} });

oldstable.commit({ subject: "YARP 3.3.2", tag: "v3.3.2" });
stable.merge(oldstable, "Merge branch yarp-3.3 into yarp-3.4");
master.merge(stable, "Merge branch yarp-3.4 into master");

master.commit({ subject: "", style: { dot: { size: '0' }} });

// Create a bugfix on stable branch
stable.commit("Start YARP 3.4.1 development");
var bugfix_on_stable = stable.branch({ name: "bugfix_on_stable", style: { lineWidth: '4' }});
bugfix_on_stable.commit("Start working on issue #xxx");
bugfix_on_stable.commit("Fix issue #xxx");
stable.merge(bugfix_on_stable, "Merge pull request #xxy from <user>/bugfix_on_stable")
master.merge(stable, "Merge branch stable into master")

master.commit({ subject: "", style: { dot: { size: '0' }} });
master.commit("...");

// Create a feature on master branch
var new_feature_on_master = master.branch({ name: "new_feature_on_master", style: { lineWidth: '4' }});
new_feature_on_master.commit("Start working on feature 'foo'");
new_feature_on_master.commit("Continue working on feature 'foo'");
new_feature_on_master.commit("Finish implementing feature 'foo'");
master.merge(new_feature_on_master, "Merge pull request #xxy from <user>/new_feature_on_master")
master.commit("Bump tweak number").tag({ name: "(v3.4.101)", style: { color: "#00aa0060", bgColor: "#fff", font: '11pt monospace'} });

master.commit({ subject: "", style: { dot: { size: '0' }} });

stable.commit({ subject: "YARP 3.4.1", tag: "v3.4.1" });
stable.commit("Start YARP 3.4.2 development");
master.merge(stable, "Merge branch stable into master")

master.commit({ subject: "", style: { dot: { size: '0' }} });
oldstable.commit({ subject: "", style: { dot: { size: '0' }} });
stable.commit({ subject: "", style: { dot: { size: '0' }} });
master.commit({ subject: "", style: { dot: { size: '0' }} });
