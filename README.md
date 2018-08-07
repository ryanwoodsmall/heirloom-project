# heirloom-project

the heirloom project provides traditional unix tools

original site: http://heirloom.sourceforge.net/

## reason

i'd like to try to get this working with static compiled musl libc

sourceforge has been getting progressively flakier and cvs access/locking is hit or mess at best

## status

### what works

compiling with a [musl-cross-make toolchain](https://github.com/richfelker/musl-cross-make) should work for:

- heirloom
- heirloom-sh
- heirloom-devtools
- heirloom-doctools

build in that order - should just be a ```make && make install```

compilation is static, meaning you need musl libc, libstdc++, etc, .a libs

### layout

everything is installed under ```/usr/local/heirloom``` by default

item                              | original path       | new path
--------------------------------- | ------------------- | --------
default (no personality) bin      | /usr/5bin           | /usr/local/heirloom/bin
default sbin                      | /usr/5bin           | /usr/local/heirloom/sbin
default lib directory             | /usr/5lib           | /usr/local/heirloom/lib
defualt man directory             | /usr/share/man/5man | /usr/local/heirloom/share/man
default files                     | /etc/default        | /usr/local/heirloom/etc/default
SVID3/SVR4-style binaries         | /usr/5bin           | /usr/local/heirloom/5bin/sv3
SVID4/SVR4.2-style binaries       | /usr/5bin/s42       | /usr/local/heirloom/5bin/s42
POSIX.2/SUS-style binaries        | /usr/5bin/posix     | /usr/local/heirloom/5bin/posix
POSIX.1-2001/SUSv3-style binaries | /usr/5bin/posix2001 | /usr/local/heirloom/5bin/posix2001
SVR4 UCB-style binaries           | /usr/ucb            | /usr/local/heirloom/ucb
UCB-style libraries               | /usr/ucblib         | /usr/local/heirloom/ucblib
development binaries              | /usr/ccs/bin        | /usr/local/heirloom/ccs/bin
development lib directory         | /usr/ccs/lib        | /usr/local/heirloom/ccs/lib
development man directory         | /usr/ccs/share/man  | /usr/local/heirloom/ccs/share/man

#### path precedence

probably something like (suit ```5bin``` subdir order to taste for your environment):

```
PATH=${PATH}:/usr/local/heirloom/5bin/posix2001
PATH=${PATH}:/usr/local/heirloom/5bin/posix
PATH=${PATH}:/usr/local/heirloom/ucb
PATH=${PATH}:/usr/local/heirloom/ccs/bin
PATH=${PATH}:/usr/local/heirloom/5bin/s42
PATH=${PATH}:/usr/local/heirloom/5bin/sv3
PATH=${PATH}:/usr/local/heirloom/sbin
PATH=${PATH}:/usr/local/heirloom/bin
export PATH
```

### what doesn't work / todo

- **heirloom-pkgtools** does _not_ work and frankly i might not bother
- ```settime``` is a broken symlink to ```touch```
- ```troff``` and ```nroff``` probably need work
  - apply NROFF/TROFF base settings from **heirloom-doctools/README** in **heirloom/man/man.dfl.in**
- ```htemp2``` in **heirloom/spell** breaks parallel make
- git submodules
  - include dedicated **netbsd-curses**: https://github.com/sabotage-linux/netbsd-curses
  - include sortix **libz**: https://sortix.org/libz/
  - include **bzip2**: http://www.bzip.org/
- unify _SUSBIN_ into _DEFBIN_?

## what isn't here

- mailx (aka **nail**): http://heirloom.sourceforge.net/mailx.html
  - initial import at: https://github.com/ryanwoodsmall/heirloom-nail.git
  - but i mean... use something else
- traditional vi (aka **ex-vi**): http://heirloom.sourceforge.net/vi.html
  - initial import at: https://github.com/ryanwoodsmall/heirloom-ex-vi.git
  - in the meantime: vim, neovim, elvis nvi, busybox vi, ...

## repo info

this is a git import of an rsync export of a cvs repository

### initial import

something like:

```
mkdir -p heirloom
cd heirloom
mkdir -p cvs
rsync -ai a.cvs.sourceforge.net::cvsroot/heirloom/. ./cvs/.
mkdir -p git
mkdir -p cvs2git-tmp
cd cvs
cvs2git --blobfile=../cvs2git-tmp/git-blob.dat --dumpfile=../cvs2git-tmp/git-dump.dat --username=cvs2git
cd ../git
git init --bare heirloom.git
cd heirloom.git
cat ../cvs2git-tmp/git-blob.dat ../cvs2git-tmp/git-dump.dat | git fast-import
git gc --prune=now
cd ..
mkdir -p clone
cd clone
git clone ../heirloom.git
cd heirloom
git remote remove origin
git remote add origin git@github.com:ryanwoodsmall/heirloom-project.git
git push -u origin master
git tag -a -m 'initial checkin of heirloom project from sourceforge rsync of cvs repos - cvs2git - github' 20180622-sf-rsync-cvs2git
git push origin --tags
```

## links

- https://sourceforge.net/p/forge/documentation/CVS/
- https://sourceforge.net/p/forge/documentation/rsync%20Backups/
- http://cvs2svn.tigris.org/cvs2git.html
