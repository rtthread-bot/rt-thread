import os
import sys


class RttAutoPR:
    def __init__(self, gh, user_name, user_email, branch_name):
        self.root = os.getcwd()
        self.gh = gh
        self.user_name = user_name
        self.user_email = user_email
        self.branch_name = branch_name

    def config(self):
        try:
            os.system('git config --global user.name  {0}'.format(self.user_name))
            os.system('git config --global user.email {0}'.format(self.user_email))
            os.system('git remote add rtt_github https://github.com/RT-Thread/rt-thread')
            os.system('git remote add rtt_gitee https://gitee.com/rtthread/rt-thread')
            os.system('git fetch rtt_github master:rtt_github')
            os.system('git fetch rtt_gitee gitee_master:rtt_gitee')
            os.system('git checkout -B master remotes/rtt_github/master')
            os.system('git merge rtt_gitee/gitee_master')
        except Exception as e:
            print('[Error] {0}'.format(e))
            return False

        return True

    def list(self):
        try:
            os.system("{0} pr list --limit 100 --repo RT-Thread/rt-thread > result.txt 2>&1".format(self.gh))
            # There is already an open PR
            with open('result.txt', 'r') as f:
                result = f.readlines()
        except Exception as e:
            print('[Error] {}'.format(e))
            return False
        robot_pr = self.user_name + ':' + self.branch_name
        print('repo branch: {0}'.format(robot_pr))
        for line in result:
            if robot_pr in line:
                print("robot PR is already open, please merge in time.")
                sys.exit(0)

    def create(self):
        try:
            result = os.system('git push --force origin master:{}'.format(self.branch_name))
            if result:
                return 1
            cmd = "{0} pr create --title '[update] RT-Thread Robot automatic submission' " \
                  "--body 'merge rt-thread gitee_master branch' " \
                  "--base master --head {1}:{2} " \
                  "--repo RT-Thread/rt-thread > result.txt".format(self.gh, self.user_name, self.branch_name)
            result = os.system(cmd)
            if result:
                return 1
        except Exception as e:
            print('[Error] {}'.format(e))
            return 1

        return 0


if __name__ == "__main__":
    try:
        gh = os.getenv('GH_PATH')
        user_name = os.getenv('USER_NAME')
        user_email = os.getenv('USER_EMAIL')
        branch_name = os.getenv('BRANCH_NAME')
    except Exception as e:
        print('[Error] {}'.format(e))
        sys.exit(1)

    robot_pr = RttAutoPR(gh, user_name, user_email, branch_name)
    # check rtt github master branch pr list
    result = robot_pr.config()
    if not result:
        sys.exit(1)
    robot_pr.list()
    sys.exit(robot_pr.create())
