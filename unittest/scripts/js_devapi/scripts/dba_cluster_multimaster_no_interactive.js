// Assumptions: check_slave_online_multimaster and check_slave_offline_multimaster
// are defined

//@ Dba: createCluster multiMaster, ok
if (__have_ssl)
  dba.createCluster('devCluster', {multiMaster: true, force: true, memberSslMode: 'REQUIRED'});
else
  dba.createCluster('devCluster', {multiMaster: true,force: true});

var Cluster = dba.getCluster('devCluster');

//@ Cluster: addInstance 2
add_instance_to_cluster(Cluster, __mysql_sandbox_port2);

wait_slave_state(Cluster, uri2, "ONLINE");

//@ Cluster: addInstance 3
add_instance_to_cluster(Cluster, __mysql_sandbox_port3);

wait_slave_state(Cluster, uri3, "ONLINE");

//@<OUT> Cluster: describe cluster with instance
Cluster.describe()

//@<OUT> Cluster: status cluster with instance
Cluster.status()

//@ Cluster: removeInstance 2
Cluster.removeInstance({host: "localhost", port:__mysql_sandbox_port2})

//@<OUT> Cluster: describe removed member
Cluster.describe()

//@<OUT> Cluster: status removed member
Cluster.status()

//@ Cluster: removeInstance 3
Cluster.removeInstance(uri3)

//@ Cluster: removeInstance 1
Cluster.removeInstance(uri1)

//@<OUT> Cluster: describe
Cluster.describe()

//@<OUT> Cluster: status
Cluster.status()

//@ Cluster: addInstance 1
add_instance_to_cluster(Cluster, __mysql_sandbox_port1);

wait_slave_state(Cluster, uri1, "ONLINE");

//@ Cluster: addInstance 2
add_instance_to_cluster(Cluster, __mysql_sandbox_port2);

wait_slave_state(Cluster, uri2, "ONLINE");

//@ Cluster: addInstance 3
add_instance_to_cluster(Cluster, __mysql_sandbox_port3);

wait_slave_state(Cluster, uri3, "ONLINE");

//@<OUT> Cluster: status: success
Cluster.status()

// Rejoin tests

//@# Dba: kill instance 3
if (__sandbox_dir)
  dba.killSandboxInstance(__mysql_sandbox_port3, {sandboxDir:__sandbox_dir})
else
  dba.killSandboxInstance(__mysql_sandbox_port3)

// XCOM needs time to kick out the member of the group. The GR team has a patch to fix this
// But won't be available for the GA release. So we need to wait until the instance is reported
// as offline
wait_slave_state(Cluster, uri3, ["OFFLINE", "UNREACHABLE"]);

//@# Dba: start instance 3
if (__sandbox_dir)
  dba.startSandboxInstance(__mysql_sandbox_port3, {sandboxDir: __sandbox_dir})
else
  dba.startSandboxInstance(__mysql_sandbox_port3)

//@ Cluster: rejoinInstance errors
Cluster.rejoinInstance();
Cluster.rejoinInstance(1,2,3);
Cluster.rejoinInstance(1);
Cluster.rejoinInstance({host: "localhost"});
Cluster.rejoinInstance({host: "localhost", schema: 'abs', authMethod:56});
Cluster.rejoinInstance("somehost:3306");

//@#: Dba: rejoin instance 3 ok
if (__have_ssl)
  Cluster.rejoinInstance({dbUser: "root", host: "localhost", port:__mysql_sandbox_port3, memberSslMode: 'REQUIRED'}, "root");
else
  Cluster.rejoinInstance({dbUser: "root", host: "localhost", port:__mysql_sandbox_port3}, "root");

wait_slave_state(Cluster, uri3, "ONLINE");

// Verify if the cluster is OK

//@<OUT> Cluster: status for rejoin: success
Cluster.status()
