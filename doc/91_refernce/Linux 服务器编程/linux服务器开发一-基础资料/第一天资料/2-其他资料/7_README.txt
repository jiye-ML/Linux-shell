
默认用户名：itcast  登录密码：itcast  可使用 sudo su 命令切换至超级用户root(学习初期不建议)


1. 如不能直接使用虚拟机，请自行安装Ubuntu操作系统。

   系统安装好以后，需联网安装一下软件，以供后续上课使用(【务必】)

	sudo apt-get install aptitude	安装另外一个与apt-get功能相近的下载工具aptitude
	sudo apt-get install vsftpd
	sudo apt-get install lftp
	sudo apt-get install openssh-server
	sudo apt-get install nfs-kernel-server
	sudo apt-get install rar
	sudo apt-get install tree
	sudo apt-get install vim


2. 如虚拟机可以正常使用，请用如下命令依次检查以上软件是否已经成功安装。

   自行安装的同学也应使用如下命令查验是否成功安装！

	sudo aptitude show tree  	使用该命令查看当前系统中是否已经安装了tree这个软件，

					如回馈信息中包含“状态：已安装”字样，说明安装无误。

   以下类似：
	sudo aptitude show vsftpd
	sudo aptitude show lftp	
	sudo aptitude show openssh-server
	sudo aptitude show nfs-kernel-server
	sudo aptitude show rar
	sudo aptitude show tree	
	sudo aptitude show vim


3.  不 建 议 随 便 升 级 操 作 系 统 版 本 ！！！