#!/bin/sh

case "$1" in
	start)
		echo "Configuring system..."
		# these are some miscellaneous stuff without a good home
		ifconfig lo 127.0.0.1 netmask 255.0.0.0 broadcast 127.255.255.255 up
		echo 1 > /proc/sys/kernel/print-fatal-signals

		# Compile-time settings
		source /rom/etc/build_profile
		if [ -z "$RDK_BUILD" ]; then
                        echo > /var/udhcpd/udhcpd.leases
		fi
		if [ -n "$BRCM_SCHED_RT_RUNTIME" ]; then
			echo $BRCM_SCHED_RT_RUNTIME > /proc/sys/kernel/sched_rt_runtime_us
		fi
		if [ -n "$BRCM_SCHED_RT_PERIOD" ]; then
			echo $BRCM_SCHED_RT_PERIOD > /proc/sys/kernel/sched_rt_period_us
		fi
		echo 0 > /proc/sys/vm/swappiness

		# set tcp_be_liberal when acceleartion is enabled
		if [ -e /proc/sys/net/netfilter/nf_conntrack_tcp_be_liberal ]; then
			echo 1 > /proc/sys/net/netfilter/nf_conntrack_tcp_be_liberal
		fi
		echo "Configuring system OK"

		exit 0
		;;

	stop)
		echo "Unconfig system not implemented..."
		exit 1
		;;

	*)
		echo "$0: unrecognized option $1"
		exit 1
		;;

esac

