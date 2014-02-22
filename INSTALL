
1. 如果在安装的时候遇到这个问题，请安装 libevent
[15:09:49 root@localhost SPECS]# rpm -Uvh fastdbcache-0.0.1-1.el6.x86_64.rpm 
error: Failed dependencies:
	libevent-1.4.so.2()(64bit) is needed by fastdbcache-0.0.1-1.el6.x86_64

[15:09:55 root@localhost SPECS]# yum list | grep libevent
libevent.i686                          1.4.13-4.el6                base         
libevent.x86_64                        1.4.13-4.el6                base         
libevent-devel.i686                    1.4.13-4.el6                base         
libevent-devel.x86_64                  1.4.13-4.el6                base 

2.FDBC 所有的文件 
[15:25:29 root@localhost SPECS]# rpm -ql fastdbcache-0.0.1-1.el6.x86_64
/etc/init.d/fdbcd
/usr/local/bin/fdbc
/usr/local/etc/fdbc.conf
/usr/share/doc/fastdbcache-0.0.1
/usr/share/doc/fastdbcache-0.0.1/ReadMe

3.

[15:25:30 root@localhost SPECS]#cat /usr/local/etc/fdbc.conf 

#server info
[Listen]
server_ip=0.0.0.0    //  fdbc 运行的IP
server_port=2345     //  fdbc 运行默认的 端口
max_openfile=1024     // 打开的 openfile

#bind use AF_UNIX
[SocketUnix]
unix_sock=0               // 默认采用 tcp 连接
path=/tmp/fdbcd.sock      // 如果 uinx_sock=1 的时候 会采用 sock 的方式连接 并在tmp上面写入sock 文件


[PostGreSQL]
pg_host=127.0.0.1           // 后端连接 PG 的ip
pg_port=5432                 // 后端连接PG的端口号

# 1 run as a daemon , 0 is no
[daemon]
do_daemonize=0              // 是否后台运行 fdbc 1 为后面运行， 0 会用户模式运行

# store a pid file
[pid]
pid_file=/var/run/fdbcd.pid      //  pid 写入的地方

# work process
[work_process]
process_num=8                  // fdbc 打开的线程数

[mem cache for select]
# 67108864 = 64M
maxbytes=67108864             // 默认对 select 的缓存为 64M

# cache for insert delete update 
[deposit]
deposit_enable_cache=on        // 是否启用 insert delete update 这三个语句缓存 默认为 on ,
                               // off 为不缓存，如果 deposit_maxbytes=0 or deposit_rule=空，也为不启用
                               // 如果 设置 deposit_quotient 值，当前连接数要大于 deposit_quotient
                               // 值才会启动，否则也不启用
deposit_method=mem             // 目前更新语句的缓存为 内存式 缓存
deposit_maxbytes=2097152       // 默认大小为2M
deposit_quotient=3             // 设置当前连接数超过 deposit_quotient 值就启用
deposit_rule={"statistics"}    // 要缓存的 表名 多个表名请用以下方式 deposit_rule={"table1","table2","table3"}


4. 启动与关闭 fdbc 过程中的情况
[15:11:17 root@localhost SPECS]# /etc/init.d/fdbcd start
Starting fdbcd service:                                    [  OK  ]

[15:35:32 root@localhost SPECS]# ps aux | grep fdbc
postgres 27825  0.0  0.0 1102932 3892 ?        Sl   15:11   0:00 /usr/local/bin/fdbc -c /usr/local/etc/fdbc.conf -d

[15:35:33 root@localhost SPECS]# netstat -atnv | grep 2345
tcp        0      0 0.0.0.0:2345                0.0.0.0:*                   LISTEN      
[15:35:59 root@localhost SPECS]# lsof -i TCP:2345
COMMAND   PID     USER   FD   TYPE  DEVICE SIZE/OFF NODE NAME
fdbc    27825 postgres   50u  IPv4 1176230      0t0  TCP *:dbm (LISTEN)

[15:36:06 root@localhost SPECS]# /etc/init.d/fdbcd stop
Stopping fdbcd service:                                    [  OK  ]

5. 通过 psql 连接FDBC及查看 FDBC 的情况
[15:37:06 root@localhost SPECS]#  psql -U vyouzhi testdb -h 127.0.0.1 -p 2345
Password for user vyouzhi: 
psql (9.3.0)
Type "help" for help.

testdb=> select version();
                                                   version                                                    
--------------------------------------------------------------------------------------------------------------
 PostgreSQL 9.3.0 on x86_64-unknown-linux-gnu, compiled by gcc (GCC) 4.4.7 20120313 (Red Hat 4.4.7-4), 64-bit
(1 row)

testdb=> cache version;
          version          
---------------------------
 fastdbcache version 0.0.1
(1 row)

testdb=> cache stat;
 count | bcount | lcount | hit | miss | set | get | bytes M | max_bytes M 
-------+--------+--------+-----+------+-----+-----+---------+-------------
 0     | 0      | 0      | 0   | 0    | 0   | 0   | 0       | 6
(1 row)

testdb=> cache help;
 command |          desc           
---------+-------------------------
 item    | found any key in memory
 stat    | check system status
 version | version for fdbc
(3 rows)

6. 简单测试
testdb=> select * from statistics limit 1;     // 查询一个语句
 id  | selects | updates | inserts | deletes 
-----+---------+---------+---------+---------
 136 |       1 |       1 |       1 |       2
(1 row)

testdb=> cache item select * from statistics limit 1;  // 发现内存还没这条语句的缓存
 key | utime | ahit | amiss 
-----+-------+------+-------
(0 rows)

testdb=> \q
[15:42:36 root@localhost SPECS]#  psql -U vyouzhi testdb -h 127.0.0.1 -p 2345    
Password for user vyouzhi: 
psql (9.3.0)
Type "help" for help.

testdb=> cache item select * from statistics limit 1;           // 当再次进入查询的时候，已发现内存中有了缓存
                key                |   utime    | ahit | amiss 
-----------------------------------+------------+------+-------
 select * from statistics limit 1; | 1389598942 | 0    | 0
(1 row)

testdb=> select * from statistics limit 1;               // 这次的结果是从缓存中出来的
 id  | selects | updates | inserts | deletes 
-----+---------+---------+---------+---------
 136 |       1 |       1 |       1 |       2
(1 row)

testdb=> cache item select * from statistics limit 1;         // 查询状态， 发现记录了一次
                key                |   utime    | ahit | amiss 
-----------------------------------+------------+------+-------
 select * from statistics limit 1; | 1389598942 | 1    | 0
(1 row)

testdb=> update statistics SET selects = 5 where id=136;      // 如果进行表的改变
UPDATE 1

[15:45:43 root@localhost SPECS]#  psql -U vyouzhi testdb -h 127.0.0.1 -p 2345
Password for user vyouzhi: 
psql (9.3.0)
Type "help" for help.

testdb=> select * from statistics where id=136;
 id  | selects | updates | inserts | deletes 
-----+---------+---------+---------+---------
 136 |       5 |       1 |       1 |       2
(1 row)

testdb=> cache item select * from statistics limit 1;                // 再次进入的时候，发现数据又从后端的PG读取数据
                key                |   utime    | ahit | amiss 
-----------------------------------+------------+------+-------
 select * from statistics limit 1; | 1389599123 | 1    | 1
(1 row)


7. php 代码测试，请看 附件 PHP 代码
