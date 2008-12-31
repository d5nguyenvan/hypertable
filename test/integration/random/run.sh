#!/bin/sh

HYPERTABLE_HOME=~/hypertable/current

$HYPERTABLE_HOME/bin/clean-database.sh
$HYPERTABLE_HOME/bin/start-all-servers.sh local

$HYPERTABLE_HOME/bin/hypertable --no-prompt < create-table.hql

echo "================="
echo "random WRITE test"
echo "================="
$HYPERTABLE_HOME/bin/random_write_test 250000000

echo "================="
echo "random READ test"
echo "================="
$HYPERTABLE_HOME/bin/random_read_test 250000000

pushd .
cd $HYPERTABLE_HOME
./bin/clean-database.sh
./bin/start-all-servers.sh local
popd

$HYPERTABLE_HOME/bin/hypertable --no-prompt < create-table-memory.hql

echo "============================="
echo "random WRITE test (IN_MEMORY)"
echo "============================="
$HYPERTABLE_HOME/bin/random_write_test 250000000

echo "============================"
echo "random READ test (IN_MEMORY)"
echo "============================"
$HYPERTABLE_HOME/bin/random_read_test 250000000
