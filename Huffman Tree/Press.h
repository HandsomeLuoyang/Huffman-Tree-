#pragma once

#pragma warning(disable:4996)
#include<cassert>
#include<string>
#include<iostream>
#include"Huffman.h"
using namespace std;
typedef long long type;
struct weight    //权值里应该包含字符出现的次数以及对应的字符和Huffman编码
{
	unsigned char flag_ch; // 代表的字符
	type ch_count; // 出现的次数
	string ch_code; // 最后的编码

	weight(type count = 0) // 构造函数，字符默认为0，编码为空
		: flag_ch(0)
		, ch_count(count)
		, ch_code("")
	{}
	weight operator+(const weight& w) // 出现次数相加
	{
		type tmp = ch_count + w.ch_count;
		return weight(tmp);
	}

	// 通过出现次数比较大小
	bool operator<(const weight& w) 
	{
		return ch_count < w.ch_count;
	}
	bool operator!=(const weight& w)
	{
		return !(ch_count == w.ch_count);
	}

};

class HuffmanPress
{
public:
	HuffmanPress() // 初始化
	{
		for (int i = 0; i < 256; i++)
		{
			weight_infos[i].flag_ch = (unsigned char)i; // 将各权的字符初始化为对应的字符
		}
	}


	bool filePress(const char* filename) // 通过文件名压缩文件
	{
		//统计出每个字符出现的次数。
		FILE* fOut = fopen(filename, "rb"); // 打开文件
		assert(fOut); // 断言，文件没有打开直接退出
		int ch = fgetc(fOut); // 获得第一个字符
		type charcount = 0;  //统计出字符出现的总次数
		while (ch != EOF) // 没有到达文件尾，一直循环
		{
			if (feof(fOut))
				break;
			weight_infos[ch].ch_count++; // 统计某个字符出现的次数
			ch = fgetc(fOut);
			charcount++; // 字符出现的总次数
		}
		weight invalid(0); // 文件尾的权
		HuffmanTree<weight> hf(weight_infos, 256, invalid);    //用得到的权重数组构建一个Huffman树
		HuffmanTreeNode<weight>* root = hf.GetRoot(); // 获得构建的树的根节点指针
		//得到Huffman编码
		string code;
		creat_all_codes(root, code);   // 通过递归为所有出现过的字符编码
		//开始压缩,创建压缩后的文件
		string CompressFilename = filename;
		CompressFilename += ".huffman"; // 压缩文件名是原文件名加上后缀
		FILE* fIn = fopen(CompressFilename.c_str(), "wb"); // 二进制打开压缩文件
		assert(fIn);
		//统计完次数使得文件指针指向了最后，所以需要使指针指向文件头
		fseek(fOut, 0, SEEK_SET);
		//向压缩文件里写入Huffman编码
		int pos = 0; // pos记录目前编码是第几位，每八位要写入一次
		char value = 0; // value记录要写进去的编码
		int ch1 = fgetc(fOut); // 原文件的第一个字符的ASCII码值
		while (ch1 != EOF)
		{
			if (feof(fOut))
				break;
			string& code = weight_infos[ch1].ch_code; // 通过原文件的ASCII码获得编码
			for (size_t i = 0; i < code.size(); i++)
			{
				value <<= 1; // 位运算，向左移一位，默认补0，例如00000011这个是3，3 << 1 之后变成00000110，这是6
				if (code[i] == '1')  //如果读到的编码是1，就进行或运算把value的最后一位也变成1
				{
					value |= 1;  // 或运算将最后一位变成 1 
				}
				if (++pos == 8)   // 满8位写入文件
				{
					fputc(value, fIn);
					value = 0;
					pos = 0;
				}
			}

			ch1 = fgetc(fOut); // 获得原文件的下一个字符，再次开始循环
		}
		if (pos)    //如果到了文件尾，最后的编码不满足一个字节
		{
			value = value << (8 - pos); // 不足一字节直接位移直到满足一字节
			fputc(value, fIn);
		}
		//将字符和字符出现的次数写进配置文件，文件解压时会用到
		string ConfigFilename = filename;
		ConfigFilename += ".config";
		FILE* fConfig = fopen(ConfigFilename.c_str(), "wb");
		assert(fConfig);
		char countStr[20];  //字符出现的次数
		//先把所有字符出现的总次数写进配置文件，因为字符出现次数很可能超过int范围，所以charcount使用的是long long 所以要分两步写入
		itoa(charcount >> 32, countStr, 10);  // 取得高位
		fputs(countStr, fConfig);  // 写入高位
		fputc('\n', fConfig); 
		itoa(charcount & 0Xffffffff, countStr, 10); //取得低位
		fputs(countStr, fConfig);  // 写入低位
		fputc('\n', fConfig);
		for (int i = 0; i < 256; i++)
		{
			string put;
			if (weight_infos[i] != invalid)
			{
				fputc(weight_infos[i].flag_ch, fConfig);//必须先把ch放进去，如果把ch作为string的字符最后转换为C的字符，会导致'\0'没有处理
				put.push_back(','); // 用 ','做分隔
				itoa(weight_infos[i].ch_count, countStr, 10); // 出现次数转换成字符串
				put += countStr; // 出现次数加入到put里面去
				fputs(put.c_str(), fConfig); // 写入字符串
				fputc('\n', fConfig); // 换行
			}
		}
		// 关闭打开的文件
		fclose(fOut);
		fclose(fIn);
		fclose(fConfig);
		return true;
	}

	bool filePress(const char* filename, int password) // 通过文件名压缩文件(有密码)
	{
		//统计出每个字符出现的次数。
		FILE* fOut = fopen(filename, "rb"); // 打开文件
		assert(fOut); // 断言，文件没有打开直接退出
		int ch = fgetc(fOut); // 获得第一个字符
		type charcount = 0;  //统计出字符出现的总次数
		while (ch != EOF) // 没有到达文件尾，一直循环
		{
			if (feof(fOut))
				break;
			weight_infos[ch].ch_count++; // 统计某个字符出现的次数
			ch = fgetc(fOut);
			charcount++; // 字符出现的总次数
		}
		weight invalid(0); // 文件尾的权
		HuffmanTree<weight> hf(weight_infos, 256, invalid);    //用得到的权重数组构建一个Huffman树
		HuffmanTreeNode<weight>* root = hf.GetRoot(); // 获得构建的树的根节点指针
		//得到Huffman编码
		string code;
		creat_all_codes(root, code);   // 通过递归为所有出现过的字符编码
		//开始压缩,创建压缩后的文件
		string CompressFilename = filename;
		CompressFilename += ".huffman"; // 压缩文件名是原文件名加上后缀
		FILE* fIn = fopen(CompressFilename.c_str(), "wb"); // 二进制打开压缩文件
		assert(fIn);
		//统计完次数使得文件指针指向了最后，所以需要使指针指向文件头
		fseek(fOut, 0, SEEK_SET);
		//向压缩文件里写入Huffman编码
		int pos = 0; // pos记录目前编码是第几位，每八位要写入一次
		char value = 0; // value记录要写进去的编码
		int ch1 = fgetc(fOut); // 原文件的第一个字符的ASCII码值
		while (ch1 != EOF)
		{
			if (feof(fOut))
				break;
			string& code = weight_infos[ch1].ch_code; // 通过原文件的ASCII码获得编码
			for (size_t i = 0; i < code.size(); i++)
			{
				value <<= 1; // 位运算，向左移一位，默认补0，例如00000011这个是3，3 << 1 之后变成00000110，这是6
				if (code[i] == '1')  //如果读到的编码是1，就进行或运算把value的最后一位也变成1
				{
					value |= 1;  // 或运算将最后一位变成 1 
				}
				if (++pos == 8)   // 满8位写入文件
				{
					fputc(value, fIn);
					value = 0;
					pos = 0;
				}
			}

			ch1 = fgetc(fOut); // 获得原文件的下一个字符，再次开始循环
		}
		if (pos)    //如果到了文件尾，最后的编码不满足一个字节
		{
			value = value << (8 - pos); // 不足一字节直接位移直到满足一字节
			fputc(value, fIn);
		}
		//将字符和字符出现的次数写进配置文件，文件解压时会用到
		string ConfigFilename = filename;
		ConfigFilename += ".config";
		FILE* fConfig = fopen(ConfigFilename.c_str(), "wb");
		assert(fConfig);

		// 写入密码
		password = password ^ 8218;  // 暗文转换
		char pswd[10];  // 存暗文密码
		itoa(password, pswd, 10);  // 密码存起来
		fputc('#', fConfig);  // # 是个flag，代表有密码
		fputs(pswd, fConfig);  // 暗文密码写入
		fputc('\n', fConfig); // 换个行


		char countStr[20];  //字符出现的次数
		//先把所有字符出现的总次数写进配置文件，因为字符出现次数很可能超过int范围，所以charcount使用的是long long 所以要分两步写入
		itoa(charcount >> 32, countStr, 10);  // 取得高位
		fputs(countStr, fConfig);  // 写入高位
		fputc('\n', fConfig);
		itoa(charcount & 0Xffffffff, countStr, 10); //取得低位
		fputs(countStr, fConfig);  // 写入低位
		fputc('\n', fConfig);
		for (int i = 0; i < 256; i++)
		{
			string put;
			if (weight_infos[i] != invalid)
			{
				fputc(weight_infos[i].flag_ch, fConfig);//必须先把ch放进去，如果把ch作为string的字符最后转换为C的字符，会导致'\0'没有处理
				put.push_back(','); // 用 ','做分隔
				itoa(weight_infos[i].ch_count, countStr, 10); // 出现次数转换成字符串
				put += countStr; // 出现次数加入到put里面去
				fputs(put.c_str(), fConfig); // 写入字符串
				fputc('\n', fConfig); // 换行
			}
		}
		// 关闭打开的文件
		fclose(fOut);
		fclose(fIn);
		fclose(fConfig);
		return true;
	}


	bool fileUncompress(const char* filename)  //这里给的是压缩文件名
	{
		//1.读取配置文件
		string ConfigFilename = filename;
		int count = ConfigFilename.rfind('.');
		ConfigFilename = ConfigFilename.substr(0, count); // 从第0个开始获得count个字符
		string UnCompressname = ConfigFilename + ".unpress";  // 用"."分割文件名，获得解压缩文件名，解压后的文件默认加上.unpress

		
		ConfigFilename += ".config"; // 获得配置文件名
		FILE* fconfig = fopen(ConfigFilename.c_str(), "rb"); // 读取配置文件
		assert(fconfig);
		FILE* fpress = fopen(filename, "rb");  // 读取压缩文件
		assert(fpress);

		type charcount = 0; //	先读出字符出现的总次数
		string line; // 从配置文件中读取一行字符
		_readLine(fconfig, line); // 读取第一行字符
		// 有密码就不干活，我这是没密码的解压函数
		if (line[0] == '#')
		{
			fclose(fconfig);
			fclose(fpress);
			cout << "你啷个回事嘛，勒个文件是有密码嘚，你不晓得选一哈，点个圈圈，在框框上头，用密码解压，莫想混顺摸鱼！" << endl;
			return false;
		}

		FILE* fUnCompress = fopen(UnCompressname.c_str(), "wb"); //创建解压缩文件
		assert(fUnCompress);


		charcount = atoi(line.c_str());  // 根据我们之前写的，第一行是字符出现的总次数的高位
		charcount <<= 32; // 所以这里需要转换一下
		line.clear(); // 清除line里面的字符
		_readLine(fconfig, line); // 读取第二行字符，第二行是字符出现的总次数的低位
		charcount += atoi(line.c_str()); // 转换成数字
		line.clear(); // 清除line里面的字符
		while (_readLine(fconfig, line))  // 读取了第一行和第二行两行特殊行之后，之后开始循环读取每一行，文件结束时feof会返回0
		{
			if (!line.empty())
			{
				char ch = line[0]; // 字符串第一位，根据我们之前写入的，肯定是代表的字符
				weight_infos[(unsigned char)ch].ch_count = atoi(line.substr(2).c_str()); // 从第二个开始获得子串，根据我们的构造，就是这个字符出现的次数了，转换成数字
				line.clear();// 清除字符串里面的内容以便读取下一行
			}
			else  //若读到一个空行，对应的字符为换行符
			{
				line += '\n';
			}
		}

		// 到这里我们已经把权重节点重新全部构造了出来，即是还原了构造的时候我们用到的weight_infos[256]，已经全部还原完毕
		//2.再次构建Huffman树
		weight invalid(0);
		HuffmanTree<weight> hf(weight_infos, 256, invalid);    //用得到的权重数组构建一个Huffman树
		HuffmanTreeNode<weight>* root = hf.GetRoot(); // 获得新构造的Huffman树的根节点
		HuffmanTreeNode<weight>* cur = root; // 建立一个用来构造树的指针
		char ch = fgetc(fpress);  // 开始从压缩文件里面读取数据
		int pos = 8; // 每个字符一个字节八位，用pos对应字节每个位与运算即可获得字节的每一位的值
		while (1)
		{
			--pos;
			if ((ch >> pos) & 1)  // ch进七位，获得从左至右的第一位的值，与 1 & 即获得这一位的值是0还是1，如果是1的话，向右节点走
			{
				cur = cur->m_pRight;
			}
			else
			{
				cur = cur->m_pLeft; // 如果是 0 的话，向左节点走
			}

			if (cur->m_pLeft == NULL && cur->m_pRight == NULL)  // 如果左右节点都为空，代表已经到达叶子结点，即已经可以确定出现了哪个字符，打印这个字符然后重新开始
			{
				fputc(cur->m_tWeight.flag_ch, fUnCompress);
				cur = root;   //再次从根节点遍历
				charcount--; // 需要打印的总字符数减1
			}
			if (pos == 0)  // 八位结束直接读取下一个字符
			{
				ch = fgetc(fpress);
				pos = 8;  // 重新设置下一个八位flag
			}
			if (charcount == 0)  //不读取压缩时为了凑够一个字节而加进去的比特位
				break;
		}

		fclose(fconfig);
		fclose(fpress);
		fclose(fUnCompress);
		return true;
	}
	bool fileUncompress(const char* filename, int password)  //这里给的是压缩文件名(有密码版)
	{
		//1.读取配置文件
		string ConfigFilename = filename;
		int count = ConfigFilename.rfind('.');
		ConfigFilename = ConfigFilename.substr(0, count); // 从第0个开始获得count个字符
		string UnCompressname = ConfigFilename + ".unpress";  // 用"."分割文件名，获得解压缩文件名，解压后的文件默认加上.unpress

		
		ConfigFilename += ".config"; // 获得配置文件名
		FILE* fconfig = fopen(ConfigFilename.c_str(), "rb"); // 读取配置文件
		assert(fconfig);
		
		FILE* fpress = fopen(filename, "rb");  // 读取压缩文件
		assert(fpress);
		string line; // 从配置文件中读取一行字符

		// 先判断密码对不对，密码不对直接不干
		_readLine(fconfig, line);  // 读取第一行
		int ans = 0; 
		for (int i = 1; i < line.size(); i++)
		{
			ans = ans * 10 + (line[i] - '0');  // 字符转化为数字，用ans记录
		}
		int fin = ans ^ 8218;  // ans与8218进行异或运算赋值给它自己
		if (password != fin)  // 如果密码不对，对不起，不能解压
		{
			cout << "你勒个密码不对哦老兄！啷个搞忘了嘛！" << endl;
			fclose(fconfig);
			fclose(fpress);
			return false;  // 直接回去
		}

		FILE* fUnCompress = fopen(UnCompressname.c_str(), "wb"); //创建解压缩文件
		assert(fUnCompress);
		line.clear();  // 密码对了，咱们有话好说，重新开始

		type charcount = 0; //	先读出字符出现的总次数
		
		_readLine(fconfig, line); // 读取第一行字符
		charcount = atoi(line.c_str());  // 根据我们之前写的，第一行是字符出现的总次数的高位
		charcount <<= 32; // 所以这里需要转换一下
		line.clear(); // 清除line里面的字符
		_readLine(fconfig, line); // 读取第二行字符，第二行是字符出现的总次数的低位
		charcount += atoi(line.c_str()); // 转换成数字
		line.clear(); // 清除line里面的字符
		while (_readLine(fconfig, line))  // 读取了第一行和第二行两行特殊行之后，之后开始循环读取每一行，文件结束时feof会返回0
		{
			if (!line.empty())
			{
				char ch = line[0]; // 字符串第一位，根据我们之前写入的，肯定是代表的字符
				weight_infos[(unsigned char)ch].ch_count = atoi(line.substr(2).c_str()); // 从第二个开始获得子串，根据我们的构造，就是这个字符出现的次数了，转换成数字
				line.clear();// 清除字符串里面的内容以便读取下一行
			}
			else  //若读到一个空行，对应的字符为换行符
			{
				line += '\n';
			}
		}

		// 到这里我们已经把权重节点重新全部构造了出来，即是还原了构造的时候我们用到的weight_infos[256]，已经全部还原完毕
		//2.再次构建Huffman树
		weight invalid(0);
		HuffmanTree<weight> hf(weight_infos, 256, invalid);    //用得到的权重数组构建一个Huffman树
		HuffmanTreeNode<weight>* root = hf.GetRoot(); // 获得新构造的Huffman树的根节点
		HuffmanTreeNode<weight>* cur = root; // 建立一个用来构造树的指针
		char ch = fgetc(fpress);  // 开始从压缩文件里面读取数据
		int pos = 8; // 每个字符一个字节八位，用pos对应字节每个位与运算即可获得字节的每一位的值
		while (1)
		{
			--pos;
			if ((ch >> pos) & 1)  // ch进七位，获得从左至右的第一位的值，与 1 & 即获得这一位的值是0还是1，如果是1的话，向右节点走
			{
				cur = cur->m_pRight;
			}
			else
			{
				cur = cur->m_pLeft; // 如果是 0 的话，向左节点走
			}

			if (cur->m_pLeft == NULL && cur->m_pRight == NULL)  // 如果左右节点都为空，代表已经到达叶子结点，即已经可以确定出现了哪个字符，打印这个字符然后重新开始
			{
				fputc(cur->m_tWeight.flag_ch, fUnCompress);
				cur = root;   //再次从根节点遍历
				charcount--; // 需要打印的总字符数减1
			}
			if (pos == 0)  // 八位结束直接读取下一个字符
			{
				ch = fgetc(fpress);
				pos = 8;  // 重新设置下一个八位flag
			}
			if (charcount == 0)  //不读取压缩时为了凑够一个字节而加进去的比特位
				break;
		}

		fclose(fconfig);
		fclose(fpress);
		fclose(fUnCompress);
		return true;
	}

protected:
	bool _readLine(FILE* filename, string& line)   // 按行读取文件
	{
		assert(filename);
		if (feof(filename))
			return false;
		unsigned char ch = fgetc(filename);

		while (ch != '\n')
		{
			line += ch;
			ch = fgetc(filename);

			if (feof(filename)) // 如果这一行已经读到文件末尾，直接返回
				//break;
				return false;
		}
		return true;
	}

	void creat_all_codes(HuffmanTreeNode<weight>* root, string code) // 通过一颗构建好的哈夫曼树生成编码
	{
		if (NULL == root)
			return;
		if (root->m_pLeft == NULL && root->m_pRight == NULL)
		{
			weight_infos[root->m_tWeight.flag_ch].ch_code = code;

		}
		creat_all_codes(root->m_pLeft, code + '0'); // 递归生成编码
		creat_all_codes(root->m_pRight, code + '1');

	}
private:
	weight weight_infos[256];
};

void testCompress()
{
	HuffmanPress hft;
	hft.filePress("test1.txt", 2345);
		//hft.filePress("git.txt");
	//	hft.filePress("1.jpg");
	//	hft.filePress("8.pdf");
		//hft.filePress("Input.BIG");
	// hft.filePress("listen.mp3");
}

void testUnCompress()
{
	HuffmanPress hf;
	hf.fileUncompress("test1.txt.huffman");
	//	hf.fileUncompress("1.jpg.huffman");
	//	hf.fileUncompress("git.txt.huffman");
	//	hf.fileUncompress("8.pdf.huffman");
		//hf.fileUncompress("Input.BIG.huffman");
	// hf.fileUncompress("listen.mp3.huffman");
}