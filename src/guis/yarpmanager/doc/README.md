# yarpmanager

`yarpmanager` is the graphical YARP tool used to edit, run, connect,
disconnect, stop and inspect applications described by XML files. It can also
start and stop the YARP name server and the `yarprun` servers used to launch
modules on local or remote machines.

When a `cluster-config.xml` file is available through the YARP resource finder,
`yarpmanager` enables the cluster panel. The cluster panel lists the configured
nodes, checks whether the YARP name server and the node `yarprun` servers are
running, and provides buttons to start, stop or kill selected `yarprun` servers
over `ssh`.

## Cluster configuration

The cluster file has a `cluster` root element, a `nameserver` element and one
or more `node` elements:

```xml
<?xml version="1.0" encoding="UTF-8"?>

<cluster name="my-cluster" user="robot">
  <nameserver namespace="/yarp" node="pc104" ssh-options="-t"/>

  <node address="pc104">/pc104</node>
  <node address="desktop" user="developer" docker="robot-container">/desktop</node>
  <node address="gpu01" conda="robot-env">/gpu01</node>
  <node address="lab01" pixi="/home/robot/workspace/my-pixi-project">/lab01</node>
</cluster>
```

Supported attributes are:

- `cluster@name`: optional cluster name.
- `cluster@user`: default ssh user for all nodes.
- `nameserver@namespace`: YARP namespace checked by the cluster panel.
- `nameserver@node`: host where `yarpserver` is started and stopped.
- `nameserver@ssh-options`: optional ssh options used for the name server host.
- `node` text: YARP port name used by `yarprun --server`.
- `node@address`: ssh host. If omitted, the node text is used as host.
- `node@user`: ssh user for this node. If omitted, `cluster@user` is used.
- `node@ssh-options`: optional ssh options for this node.
- `node@display`: optional value exported as `DISPLAY` before starting
  `yarprun`.
- `node@docker`: Docker container name where `yarprun` is started.
- `node@conda`: Conda environment name activated before starting `yarprun`.
- `node@pixi`: Pixi project directory used to start `yarprun`.

Only one runtime attribute among `docker`, `conda` and `pixi` can be used for a
node.

## Starting `yarprun` in a runtime

For each node, the cluster table shows the node status, YARP node name, display,
ssh user, address, runtime, runtime target and log option. The runtime and
target columns can be edited before pressing **Run selected**.

If no runtime is selected, `yarpmanager` starts:

```sh
yarprun --server /node
```

If `docker` is selected, `yarpmanager` first checks that the configured
container is running on the node host, then starts `yarprun` inside it:

```sh
docker exec <container> sh -lc 'yarprun --server /node'
```

If `conda` is selected, `yarpmanager` opens a login shell, sources `conda.sh`
from one of the standard installation paths, activates the configured
environment and starts `yarprun` inside it.

If `pixi` is selected, `yarpmanager` checks that the configured directory exists,
contains a `pixi.toml`, and that `pixi` is available on the remote host. It then
runs `yarprun` with:

```sh
pixi run yarprun --server /node
```

When the log checkbox is enabled, `yarprun --log` is used. Startup output is
stored on the remote host in `/tmp/yarpmanager_yarprun_<node>.log`. If startup
fails, `yarpmanager` reports the tail of that log to help diagnose the problem.

## Docker container controls

The cluster panel also contains a **Docker containers** table. It lists the
Docker containers referenced by the cluster nodes, grouped by host and ssh
settings, and shows their current status and container id.

The available actions are:

- **Refresh**: reload the Docker container status from the remote hosts.
- **Start**: run `docker start <container>` for each selected container.
- **Stop**: run `docker stop <container>` for each selected container.

Docker-backed `yarprun` nodes are highlighted in the cluster table when their
target container is running. A Docker-backed node cannot be started until its
container is running.

## Name server and node management

The name server controls start or stop `yarpserver` on the configured
`nameserver@node`. The cluster panel checks the configured namespace to decide
whether the server is available.

The node controls operate on the selected cluster rows:

- **Check all**: check whether all configured `yarprun` servers are reachable.
- **Run selected**: start `yarprun` on the selected nodes, using the selected
  runtime if present.
- **Stop selected**: ask the selected `yarprun` servers to exit with
  `yarprun --exit --on /node`.
- **Kill selected**: forcibly terminate `yarprun` on the selected hosts with
  `killall -9 yarprun`.

The command execution area can be used to run an arbitrary command on a selected
configured node through `ssh`.
