
show-config:
	cat /sys/kernel/mm/transparent_hugepage/enabled

check-hugepage-current-data:
	cat /proc/meminfo

check-counter:
	cat /proc/sys/vm/nr_hugepages

get-huge-pages:
	echo 5 | sudo tee /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages

release-huge-pages:
	echo 0 | sudo tee /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
