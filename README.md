LanDnD
========
LanDnD is a simple utility that allow you to share files with PCs that are in your LAN. _(and not only)_<br>
You can choose if you want to type the relative path, the absolute path or (if you want) you can drag & drop the file(s) into the terminal.<br>
LanDnD is developed by:

* Paolo Stivanin a.k.a. Polslinux


Version
-------
The **last stable** version is the **v1.1.0** and you can find it in the master branch.<br>
If you want to use the *dev release* you have to do the following things:<br>

- `git clone https://github.com/polslinux/LanDND.git`<br>
- `cd LanDND`<br>
- `git checkout experimental`<br>

and then follow the instructions that are in *Compiling (only the last)*.

RoadMap
-------
<empty>

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
* compile LanDND with:<br>
`make`<br>
When the compile phase finished, you will find the `landnd` ELF into the project root directory.

How to use landnd
-----------------
`landnd --send <ip>` to send a file to the given ip<br>
`landnd --recv [y|n]` to receive files. **y** if you want to auto accept all the incoming files or **n** if you want to confirm every file before receive it.<br>

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
