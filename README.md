LanDnD
========
LanDnD is a simple utility that allow you to share files with PCs that are in your LAN. _(and not only)_<br>
You can choose if you want to type the relative path, the absolute path or (if you want) you can drag & drop the file(s) into the terminal.<br>
LanDnD is developed by:

* Paolo Stivanin a.k.a. Polslinux


Version
-------
The **last stable** version is the **v1.0.0** and you can download it from: https://github.com/polslinux/LanDND/zipball/v1.0.0<br>
If you want to use the *dev release* you have to do the following things:<br>

- `git clone https://github.com/polslinux/LanDND.git`<br>
- `cd LanDND`<br>

and then follow the instructions that are in *Compiling (from number 3)*.

RoadMap
-------
??/??/2013 - v1.1.0
* ADDED: md5sum check for each transferred file [✓]
* IMPROVED: relative path cannot be used due to security reason
* IMPROVED: better error handling and reporting
* IMPROVED: usability (simpler argv options, server ip must be given as argv parameter) [✓]

Requirements
------------

* GCC or Clang --> _(if you use Clang you **must use** a version **>= 3.1**)_
* make
* GNU/Linux -----> _(LanDND **may** works under Mac OSX but it **won't works** under MS Windows)_
* nmap ----------> _(if you want to use the '--list-ip' option)_

Compiling
----------------------------------------------------------------
* Clone the repo<br>
`git clone https://github.com/polslinux/LanDND.git`<br>
* move inside the directory you've just downloaded:<br>
`cd $PWD/LanDND`<br>
* give the right permission to autogen.sh:<br>
`chmod 744 autogen.sh`<br>
* and now execute the script:<br>
`./autogen.sh`<br>
When the script finished you will find the `landnd` ELF into the project root directory.

How to use landnd
-----------------
`landnd --send <ip>` to send a file to the given ip<br>
`landnd --recv [y|n]` to receive files. **y** if you want to auto accept all the incoming files or **n** if you want to confirm every file before download it.<br>

Extra options
-------------
`landnd --list-ip <ip>` list all the ip connected to your lan<br>
`landnd --version` show the version info and the developer info<br>
`landnd --help` show up a simple help<br>

License
-------
All the code is released under the license GNU AGPL v3 and above.<br>
<http://www.gnu.org/licenses/agpl-3.0.html><br>

Web Site
--------
Developer web site:	<http://www.paolostivanin.com><br>
Project web site:	<https://github.com/polslinux/LanDND>
