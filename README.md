PolShare
========
PolShare is a simple utility that allows you to share one or more file between multiple PC<br>
PolShare is developed by

* Paolo Stivanin a.k.a. Polslinux


Version
-------
The **latest stable** release can be found inside the master branch.<br>
If you want to use the *dev release* you have to do the following things:<br>

- `git clone https://github.com/polslinux/polshare.git`<br>
- `cd polshare`<br>
- `git checkout experimental`<br>

and then follow the instructions that are in *Compiling (only the last)*.


Requirements
------------

* GCC or Clang --> _(if you use Clang you **must use** a version **>= 3.1**)_
* make
* GNU/Linux -----> _(polshare **may** works under Mac OSX but it **won't works** under MS Windows)_
* nmap ----------> _(if you want to use the '--list-ip' option)_


Compiling
----------------------------------------------------------------
* Clone the repo<br>
`git clone https://github.com/polslinux/polshare.git`<br>
* move inside the directory you've just downloaded:<br>
`cd $PWD/polshare`<br>
* compile polshare with:<br>
`make`<br>
When the compile phase finished, you will find the `polshare` ELF into the project root directory.


How to use polshare
-----------------
`polshare -s | --send <ip>` to send a file to the given ip<br>
`polshare -r | --recv [y|n]` to receive files. **y** if you want to auto accept all the incoming files or **n** if you want to confirm every file before receive it.<br>


Extra options
-------------
`polshare -i | --list-ip <ip>` list all the ip connected to your lan<br>
`polshare -v | --version` show the version info and the developer info<br>
`polshare -h | --help` show up a simple help<br>


License
-------
All the code is released under the license GNU AGPL v3 and above.<br>
<http://www.gnu.org/licenses/agpl-3.0.html><br>


Web Site
--------
Developer web site:	<https://www.paolostivanin.com>
