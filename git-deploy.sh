## Задача: реализовать автоматическую починку и выкладку на бой старых проектов методом git pull
## при условии что доступ к серверам имеют другие разработчики и они периодически вносят правки
## прямо на бою, ломают репозиторий и боевой сервер
## Этот скрипт удобно повесить в crontab, либо запускать из другого демона


LOG=$(pwd)/git-deploy.log
WORKING_DIRECTORY=$1

function git_last_commit_info () {
	git log -1 --oneline $1 --
}

function git_last_commit_hash () {
	git_last_commit_info $1 | cut -d ' ' -f 1
}

function git_fetch () {
	date --rfc-2822
	git fetch --verbose
	git stash save --include-untracked
}

function git_pull () {
	git_last_commit_info master
	git_last_commit_info origin/master
	git rebase --stat origin/master
	git rebase --abort
}


cd $WORKING_DIRECTORY
git_fetch 2>&1 >> $LOG

master=$(git_last_commit_hash master)
origin=$(git_last_commit_hash origin/master)

if [ "$master" != "$origin" ]
then
	git_pull 2>&1 >> $LOG
fi
