LanDnD
========
LanDnD is a simple utility that allow you to share files with PCs that are in your LAN.<br>
You could type the path of the file or you can drag 'n drop it into the terminal.<br>
LanDnD is developed by:

* Paolo Stivanin a.k.a. Polslinux


Version (no stable version at the moment)
-----------------------------------------
The **last and stable** version is the **XXX** and you can download it from: https://github.com/polslinux/LanDND/zipball/XXX<br>
If you want to use the *dev release* you have to do the following things:<br>

- `git clone https://github.com/polslinux/LanDND.git`<br>
- `cd LanDND`<br>
- `git checkout experimental`<br>

and then follow the instructions that are in *Compiling (from number 3)*.

Requirements
------------

* GCC or Clang	_(if you use Clang you **must use** a version **>= 3.1**)_
* Make
* GNU/Linux		_(LanDND may works under Mac OSX but it **won't works** under MS Windows)_
* Build Tools	_(Autoconf and Automake)_
* Nmap			_(if you want to use the '-s' option)_

Compiling (incomplete. Just type make on the project root)
----------------------------------------------------------------
* Clone the repo<br>
`git clone https://github.com/polslinux/LanDND.git`<br>
* move inside the directory you've just downloaded:<br>
`cd $PWD/LanDND`<br>
* give the right permission to autogen.sh:<br>
`chmod 744 autogen.sh`<br>
* and now execute the script:<br>
`./autogen.sh`<br>
When the script finished you will find `landnd` into the project root directory.

How to use landnd
-----------------
`landnd` is simple to use.<br>
First of all you have to start it with:<br>
`./landnd`<br>
then you have to choose if you want to send a file (number 1) or to receive a file (number 2).<br>

Extra options
-------------
`landnd -s` will search _(thanks to **nmap**)_ all the active hosts in your lan and it will print their IPv4.<br>
`landnd -h` show up a simple help.

License
-------
All the code is released under the license GNU AGPL v3 and above.<br>
<http://www.gnu.org/licenses/agpl-3.0.html><br>

Web Site
--------
Developer web site:	<http://www.polslinux.it><br>
Project web site:	<https://github.com/polslinux/LanDND>
