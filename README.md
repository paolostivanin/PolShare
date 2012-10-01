LanDnD
========
LanDnD is a simple utility that allow you to share files with PCs that are in your LAN. _(and not only)_<br>
You can choose if you want to type the relative path, the absolute path or (if you want) you can drag & drop the file(s) into the terminal.<br>
LanDnD is developed by:

* Paolo Stivanin a.k.a. Polslinux


Version
-------
The **last and stable** version is the **v1.0.0** and you can download it from: https://github.com/polslinux/LanDND/zipball/v1.0.0<br>
If you want to use the *dev release* you have to do the following things:<br>

- `git clone https://github.com/polslinux/LanDND.git`<br>
- `cd LanDND`<br>

and then follow the instructions that are in *Compiling (from number 3)*.

Requirements
------------

* GCC or Clang --> _(if you use Clang you **must use** a version **>= 3.1**)_
* Make
* GNU/Linux -----> _(LanDND **may** works under Mac OSX but it **won't works** under MS Windows)_
* Build Tools ---> _(Autoconf and Automake)_
* Nmap ----------> _(if you want to use the '-s' option)_

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
`landnd` has two startup modes: client mode (1) or server mode (2).<br>
`landnd 1` allow you to send files<br>
`landnd 2` allow you to receive files.<br>
You can use both relative and absolute path when you are writing or dragging & dropping your input files BUT if you want to use the **relative path** you **MUST** have to prefix the relative path name or the file name with a **@**  _(e.g. @Documents/file or @file)_

Extra options
-------------
`landnd -s` search to all the active hosts into your LAN and print their IPv4 _(thanks to **nmap**)_<br>
`landnd -ya` start landnd **server (2)** with "yes to all confirmation" so you don't have to write Y to all the incoming files.
`landnd -sya` the above options merged into one<br>
`landnd -v` show the version info and the developer info<br>
`landnd -h` show up a simple help<br>

License
-------
All the code is released under the license GNU AGPL v3 and above.<br>
<http://www.gnu.org/licenses/agpl-3.0.html><br>

Web Site
--------
Developer web site:	<http://www.polslinux.it><br>
Project web site:	<https://github.com/polslinux/LanDND>
