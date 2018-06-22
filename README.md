# heirloom-project

the heirloom project provides traditional unix tools

original site: http://heirloom.sourceforge.net/

## repo info

this is a git import of an rsync export of a cvs repository

## reason

i'd like to try to get this working with static compiled musl libc

sourceforge has been getting progressively flakier and cvs access/locking is hit or mess at best

### initial import

something like:

```
mkdir -p heirloom
cd heirloom
mkdir -p cvs
rsync -ai a.cvs.sourceforge.net::cvsroot/heirloom/. ./cvs/.
mkdir -p git
cd git
mkdir -p cvs2git-tmp
cvs2git --blobfile=cvs2git-tmp/git-blob.dat --dumpfile=cvs2git-tmp/git-dump.dat --username=cvs2git
git init --bar heirloom.git
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

#### links

- https://sourceforge.net/p/forge/documentation/CVS/
- https://sourceforge.net/p/forge/documentation/rsync%20Backups/
- http://cvs2svn.tigris.org/cvs2git.html
