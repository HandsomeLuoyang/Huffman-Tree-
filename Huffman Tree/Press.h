#pragma once

#pragma warning(disable:4996)
#include<cassert>
#include<string>
#include<iostream>
#include"Huffman.h"
using namespace std;
typedef long long type;
struct weight    //Ȩֵ��Ӧ�ð����ַ����ֵĴ����Լ���Ӧ���ַ���Huffman����
{
	unsigned char flag_ch; // ������ַ�
	type ch_count; // ���ֵĴ���
	string ch_code; // ���ı���

	weight(type count = 0) // ���캯�����ַ�Ĭ��Ϊ0������Ϊ��
		: flag_ch(0)
		, ch_count(count)
		, ch_code("")
	{}
	weight operator+(const weight& w) // ���ִ������
	{
		type tmp = ch_count + w.ch_count;
		return weight(tmp);
	}

	// ͨ�����ִ����Ƚϴ�С
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
	HuffmanPress() // ��ʼ��
	{
		for (int i = 0; i < 256; i++)
		{
			weight_infos[i].flag_ch = (unsigned char)i; // ����Ȩ���ַ���ʼ��Ϊ��Ӧ���ַ�
		}
	}


	bool filePress(const char* filename) // ͨ���ļ���ѹ���ļ�
	{
		//ͳ�Ƴ�ÿ���ַ����ֵĴ�����
		FILE* fOut = fopen(filename, "rb"); // ���ļ�
		assert(fOut); // ���ԣ��ļ�û�д�ֱ���˳�
		int ch = fgetc(fOut); // ��õ�һ���ַ�
		type charcount = 0;  //ͳ�Ƴ��ַ����ֵ��ܴ���
		while (ch != EOF) // û�е����ļ�β��һֱѭ��
		{
			if (feof(fOut))
				break;
			weight_infos[ch].ch_count++; // ͳ��ĳ���ַ����ֵĴ���
			ch = fgetc(fOut);
			charcount++; // �ַ����ֵ��ܴ���
		}
		weight invalid(0); // �ļ�β��Ȩ
		HuffmanTree<weight> hf(weight_infos, 256, invalid);    //�õõ���Ȩ�����鹹��һ��Huffman��
		HuffmanTreeNode<weight>* root = hf.GetRoot(); // ��ù��������ĸ��ڵ�ָ��
		//�õ�Huffman����
		string code;
		creat_all_codes(root, code);   // ͨ���ݹ�Ϊ���г��ֹ����ַ�����
		//��ʼѹ��,����ѹ������ļ�
		string CompressFilename = filename;
		CompressFilename += ".huffman"; // ѹ���ļ�����ԭ�ļ������Ϻ�׺
		FILE* fIn = fopen(CompressFilename.c_str(), "wb"); // �����ƴ�ѹ���ļ�
		assert(fIn);
		//ͳ�������ʹ���ļ�ָ��ָ�������������Ҫʹָ��ָ���ļ�ͷ
		fseek(fOut, 0, SEEK_SET);
		//��ѹ���ļ���д��Huffman����
		int pos = 0; // pos��¼Ŀǰ�����ǵڼ�λ��ÿ��λҪд��һ��
		char value = 0; // value��¼Ҫд��ȥ�ı���
		int ch1 = fgetc(fOut); // ԭ�ļ��ĵ�һ���ַ���ASCII��ֵ
		while (ch1 != EOF)
		{
			if (feof(fOut))
				break;
			string& code = weight_infos[ch1].ch_code; // ͨ��ԭ�ļ���ASCII���ñ���
			for (size_t i = 0; i < code.size(); i++)
			{
				value <<= 1; // λ���㣬������һλ��Ĭ�ϲ�0������00000011�����3��3 << 1 ֮����00000110������6
				if (code[i] == '1')  //��������ı�����1���ͽ��л������value�����һλҲ���1
				{
					value |= 1;  // �����㽫���һλ��� 1 
				}
				if (++pos == 8)   // ��8λд���ļ�
				{
					fputc(value, fIn);
					value = 0;
					pos = 0;
				}
			}

			ch1 = fgetc(fOut); // ���ԭ�ļ�����һ���ַ����ٴο�ʼѭ��
		}
		if (pos)    //��������ļ�β�����ı��벻����һ���ֽ�
		{
			value = value << (8 - pos); // ����һ�ֽ�ֱ��λ��ֱ������һ�ֽ�
			fputc(value, fIn);
		}
		//���ַ����ַ����ֵĴ���д�������ļ����ļ���ѹʱ���õ�
		string ConfigFilename = filename;
		ConfigFilename += ".config";
		FILE* fConfig = fopen(ConfigFilename.c_str(), "wb");
		assert(fConfig);
		char countStr[20];  //�ַ����ֵĴ���
		//�Ȱ������ַ����ֵ��ܴ���д�������ļ�����Ϊ�ַ����ִ����ܿ��ܳ���int��Χ������charcountʹ�õ���long long ����Ҫ������д��
		itoa(charcount >> 32, countStr, 10);  // ȡ�ø�λ
		fputs(countStr, fConfig);  // д���λ
		fputc('\n', fConfig); 
		itoa(charcount & 0Xffffffff, countStr, 10); //ȡ�õ�λ
		fputs(countStr, fConfig);  // д���λ
		fputc('\n', fConfig);
		for (int i = 0; i < 256; i++)
		{
			string put;
			if (weight_infos[i] != invalid)
			{
				fputc(weight_infos[i].flag_ch, fConfig);//�����Ȱ�ch�Ž�ȥ�������ch��Ϊstring���ַ����ת��ΪC���ַ����ᵼ��'\0'û�д���
				put.push_back(','); // �� ','���ָ�
				itoa(weight_infos[i].ch_count, countStr, 10); // ���ִ���ת�����ַ���
				put += countStr; // ���ִ������뵽put����ȥ
				fputs(put.c_str(), fConfig); // д���ַ���
				fputc('\n', fConfig); // ����
			}
		}
		// �رմ򿪵��ļ�
		fclose(fOut);
		fclose(fIn);
		fclose(fConfig);
		return true;
	}

	bool filePress(const char* filename, int password) // ͨ���ļ���ѹ���ļ�(������)
	{
		//ͳ�Ƴ�ÿ���ַ����ֵĴ�����
		FILE* fOut = fopen(filename, "rb"); // ���ļ�
		assert(fOut); // ���ԣ��ļ�û�д�ֱ���˳�
		int ch = fgetc(fOut); // ��õ�һ���ַ�
		type charcount = 0;  //ͳ�Ƴ��ַ����ֵ��ܴ���
		while (ch != EOF) // û�е����ļ�β��һֱѭ��
		{
			if (feof(fOut))
				break;
			weight_infos[ch].ch_count++; // ͳ��ĳ���ַ����ֵĴ���
			ch = fgetc(fOut);
			charcount++; // �ַ����ֵ��ܴ���
		}
		weight invalid(0); // �ļ�β��Ȩ
		HuffmanTree<weight> hf(weight_infos, 256, invalid);    //�õõ���Ȩ�����鹹��һ��Huffman��
		HuffmanTreeNode<weight>* root = hf.GetRoot(); // ��ù��������ĸ��ڵ�ָ��
		//�õ�Huffman����
		string code;
		creat_all_codes(root, code);   // ͨ���ݹ�Ϊ���г��ֹ����ַ�����
		//��ʼѹ��,����ѹ������ļ�
		string CompressFilename = filename;
		CompressFilename += ".huffman"; // ѹ���ļ�����ԭ�ļ������Ϻ�׺
		FILE* fIn = fopen(CompressFilename.c_str(), "wb"); // �����ƴ�ѹ���ļ�
		assert(fIn);
		//ͳ�������ʹ���ļ�ָ��ָ�������������Ҫʹָ��ָ���ļ�ͷ
		fseek(fOut, 0, SEEK_SET);
		//��ѹ���ļ���д��Huffman����
		int pos = 0; // pos��¼Ŀǰ�����ǵڼ�λ��ÿ��λҪд��һ��
		char value = 0; // value��¼Ҫд��ȥ�ı���
		int ch1 = fgetc(fOut); // ԭ�ļ��ĵ�һ���ַ���ASCII��ֵ
		while (ch1 != EOF)
		{
			if (feof(fOut))
				break;
			string& code = weight_infos[ch1].ch_code; // ͨ��ԭ�ļ���ASCII���ñ���
			for (size_t i = 0; i < code.size(); i++)
			{
				value <<= 1; // λ���㣬������һλ��Ĭ�ϲ�0������00000011�����3��3 << 1 ֮����00000110������6
				if (code[i] == '1')  //��������ı�����1���ͽ��л������value�����һλҲ���1
				{
					value |= 1;  // �����㽫���һλ��� 1 
				}
				if (++pos == 8)   // ��8λд���ļ�
				{
					fputc(value, fIn);
					value = 0;
					pos = 0;
				}
			}

			ch1 = fgetc(fOut); // ���ԭ�ļ�����һ���ַ����ٴο�ʼѭ��
		}
		if (pos)    //��������ļ�β�����ı��벻����һ���ֽ�
		{
			value = value << (8 - pos); // ����һ�ֽ�ֱ��λ��ֱ������һ�ֽ�
			fputc(value, fIn);
		}
		//���ַ����ַ����ֵĴ���д�������ļ����ļ���ѹʱ���õ�
		string ConfigFilename = filename;
		ConfigFilename += ".config";
		FILE* fConfig = fopen(ConfigFilename.c_str(), "wb");
		assert(fConfig);

		// д������
		password = password ^ 8218;  // ����ת��
		char pswd[10];  // �氵������
		itoa(password, pswd, 10);  // ���������
		fputc('#', fConfig);  // # �Ǹ�flag������������
		fputs(pswd, fConfig);  // ��������д��
		fputc('\n', fConfig); // ������


		char countStr[20];  //�ַ����ֵĴ���
		//�Ȱ������ַ����ֵ��ܴ���д�������ļ�����Ϊ�ַ����ִ����ܿ��ܳ���int��Χ������charcountʹ�õ���long long ����Ҫ������д��
		itoa(charcount >> 32, countStr, 10);  // ȡ�ø�λ
		fputs(countStr, fConfig);  // д���λ
		fputc('\n', fConfig);
		itoa(charcount & 0Xffffffff, countStr, 10); //ȡ�õ�λ
		fputs(countStr, fConfig);  // д���λ
		fputc('\n', fConfig);
		for (int i = 0; i < 256; i++)
		{
			string put;
			if (weight_infos[i] != invalid)
			{
				fputc(weight_infos[i].flag_ch, fConfig);//�����Ȱ�ch�Ž�ȥ�������ch��Ϊstring���ַ����ת��ΪC���ַ����ᵼ��'\0'û�д���
				put.push_back(','); // �� ','���ָ�
				itoa(weight_infos[i].ch_count, countStr, 10); // ���ִ���ת�����ַ���
				put += countStr; // ���ִ������뵽put����ȥ
				fputs(put.c_str(), fConfig); // д���ַ���
				fputc('\n', fConfig); // ����
			}
		}
		// �رմ򿪵��ļ�
		fclose(fOut);
		fclose(fIn);
		fclose(fConfig);
		return true;
	}


	bool fileUncompress(const char* filename)  //���������ѹ���ļ���
	{
		//1.��ȡ�����ļ�
		string ConfigFilename = filename;
		int count = ConfigFilename.rfind('.');
		ConfigFilename = ConfigFilename.substr(0, count); // �ӵ�0����ʼ���count���ַ�
		string UnCompressname = ConfigFilename + ".unpress";  // ��"."�ָ��ļ�������ý�ѹ���ļ�������ѹ����ļ�Ĭ�ϼ���.unpress

		
		ConfigFilename += ".config"; // ��������ļ���
		FILE* fconfig = fopen(ConfigFilename.c_str(), "rb"); // ��ȡ�����ļ�
		assert(fconfig);
		FILE* fpress = fopen(filename, "rb");  // ��ȡѹ���ļ�
		assert(fpress);

		type charcount = 0; //	�ȶ����ַ����ֵ��ܴ���
		string line; // �������ļ��ж�ȡһ���ַ�
		_readLine(fconfig, line); // ��ȡ��һ���ַ�
		// ������Ͳ��ɻ������û����Ľ�ѹ����
		if (line[0] == '#')
		{
			fclose(fconfig);
			fclose(fpress);
			cout << "��ॸ�������ո��ļ���������N���㲻����ѡһ�������ȦȦ���ڿ����ͷ���������ѹ��Ī���˳���㣡" << endl;
			return false;
		}

		FILE* fUnCompress = fopen(UnCompressname.c_str(), "wb"); //������ѹ���ļ�
		assert(fUnCompress);


		charcount = atoi(line.c_str());  // ��������֮ǰд�ģ���һ�����ַ����ֵ��ܴ����ĸ�λ
		charcount <<= 32; // ����������Ҫת��һ��
		line.clear(); // ���line������ַ�
		_readLine(fconfig, line); // ��ȡ�ڶ����ַ����ڶ������ַ����ֵ��ܴ����ĵ�λ
		charcount += atoi(line.c_str()); // ת��������
		line.clear(); // ���line������ַ�
		while (_readLine(fconfig, line))  // ��ȡ�˵�һ�к͵ڶ�������������֮��֮��ʼѭ����ȡÿһ�У��ļ�����ʱfeof�᷵��0
		{
			if (!line.empty())
			{
				char ch = line[0]; // �ַ�����һλ����������֮ǰд��ģ��϶��Ǵ�����ַ�
				weight_infos[(unsigned char)ch].ch_count = atoi(line.substr(2).c_str()); // �ӵڶ�����ʼ����Ӵ����������ǵĹ��죬��������ַ����ֵĴ����ˣ�ת��������
				line.clear();// ����ַ�������������Ա��ȡ��һ��
			}
			else  //������һ�����У���Ӧ���ַ�Ϊ���з�
			{
				line += '\n';
			}
		}

		// �����������Ѿ���Ȩ�ؽڵ�����ȫ�������˳��������ǻ�ԭ�˹����ʱ�������õ���weight_infos[256]���Ѿ�ȫ����ԭ���
		//2.�ٴι���Huffman��
		weight invalid(0);
		HuffmanTree<weight> hf(weight_infos, 256, invalid);    //�õõ���Ȩ�����鹹��һ��Huffman��
		HuffmanTreeNode<weight>* root = hf.GetRoot(); // ����¹����Huffman���ĸ��ڵ�
		HuffmanTreeNode<weight>* cur = root; // ����һ��������������ָ��
		char ch = fgetc(fpress);  // ��ʼ��ѹ���ļ������ȡ����
		int pos = 8; // ÿ���ַ�һ���ֽڰ�λ����pos��Ӧ�ֽ�ÿ��λ�����㼴�ɻ���ֽڵ�ÿһλ��ֵ
		while (1)
		{
			--pos;
			if ((ch >> pos) & 1)  // ch����λ����ô������ҵĵ�һλ��ֵ���� 1 & �������һλ��ֵ��0����1�������1�Ļ������ҽڵ���
			{
				cur = cur->m_pRight;
			}
			else
			{
				cur = cur->m_pLeft; // ����� 0 �Ļ�������ڵ���
			}

			if (cur->m_pLeft == NULL && cur->m_pRight == NULL)  // ������ҽڵ㶼Ϊ�գ������Ѿ�����Ҷ�ӽ�㣬���Ѿ�����ȷ���������ĸ��ַ�����ӡ����ַ�Ȼ�����¿�ʼ
			{
				fputc(cur->m_tWeight.flag_ch, fUnCompress);
				cur = root;   //�ٴδӸ��ڵ����
				charcount--; // ��Ҫ��ӡ�����ַ�����1
			}
			if (pos == 0)  // ��λ����ֱ�Ӷ�ȡ��һ���ַ�
			{
				ch = fgetc(fpress);
				pos = 8;  // ����������һ����λflag
			}
			if (charcount == 0)  //����ȡѹ��ʱΪ�˴չ�һ���ֽڶ��ӽ�ȥ�ı���λ
				break;
		}

		fclose(fconfig);
		fclose(fpress);
		fclose(fUnCompress);
		return true;
	}
	bool fileUncompress(const char* filename, int password)  //���������ѹ���ļ���(�������)
	{
		//1.��ȡ�����ļ�
		string ConfigFilename = filename;
		int count = ConfigFilename.rfind('.');
		ConfigFilename = ConfigFilename.substr(0, count); // �ӵ�0����ʼ���count���ַ�
		string UnCompressname = ConfigFilename + ".unpress";  // ��"."�ָ��ļ�������ý�ѹ���ļ�������ѹ����ļ�Ĭ�ϼ���.unpress

		
		ConfigFilename += ".config"; // ��������ļ���
		FILE* fconfig = fopen(ConfigFilename.c_str(), "rb"); // ��ȡ�����ļ�
		assert(fconfig);
		
		FILE* fpress = fopen(filename, "rb");  // ��ȡѹ���ļ�
		assert(fpress);
		string line; // �������ļ��ж�ȡһ���ַ�

		// ���ж�����Բ��ԣ����벻��ֱ�Ӳ���
		_readLine(fconfig, line);  // ��ȡ��һ��
		int ans = 0; 
		for (int i = 1; i < line.size(); i++)
		{
			ans = ans * 10 + (line[i] - '0');  // �ַ�ת��Ϊ���֣���ans��¼
		}
		int fin = ans ^ 8218;  // ans��8218����������㸳ֵ�����Լ�
		if (password != fin)  // ������벻�ԣ��Բ��𣬲��ܽ�ѹ
		{
			cout << "���ո����벻��Ŷ���֣�ॸ��������" << endl;
			fclose(fconfig);
			fclose(fpress);
			return false;  // ֱ�ӻ�ȥ
		}

		FILE* fUnCompress = fopen(UnCompressname.c_str(), "wb"); //������ѹ���ļ�
		assert(fUnCompress);
		line.clear();  // ������ˣ������л���˵�����¿�ʼ

		type charcount = 0; //	�ȶ����ַ����ֵ��ܴ���
		
		_readLine(fconfig, line); // ��ȡ��һ���ַ�
		charcount = atoi(line.c_str());  // ��������֮ǰд�ģ���һ�����ַ����ֵ��ܴ����ĸ�λ
		charcount <<= 32; // ����������Ҫת��һ��
		line.clear(); // ���line������ַ�
		_readLine(fconfig, line); // ��ȡ�ڶ����ַ����ڶ������ַ����ֵ��ܴ����ĵ�λ
		charcount += atoi(line.c_str()); // ת��������
		line.clear(); // ���line������ַ�
		while (_readLine(fconfig, line))  // ��ȡ�˵�һ�к͵ڶ�������������֮��֮��ʼѭ����ȡÿһ�У��ļ�����ʱfeof�᷵��0
		{
			if (!line.empty())
			{
				char ch = line[0]; // �ַ�����һλ����������֮ǰд��ģ��϶��Ǵ�����ַ�
				weight_infos[(unsigned char)ch].ch_count = atoi(line.substr(2).c_str()); // �ӵڶ�����ʼ����Ӵ����������ǵĹ��죬��������ַ����ֵĴ����ˣ�ת��������
				line.clear();// ����ַ�������������Ա��ȡ��һ��
			}
			else  //������һ�����У���Ӧ���ַ�Ϊ���з�
			{
				line += '\n';
			}
		}

		// �����������Ѿ���Ȩ�ؽڵ�����ȫ�������˳��������ǻ�ԭ�˹����ʱ�������õ���weight_infos[256]���Ѿ�ȫ����ԭ���
		//2.�ٴι���Huffman��
		weight invalid(0);
		HuffmanTree<weight> hf(weight_infos, 256, invalid);    //�õõ���Ȩ�����鹹��һ��Huffman��
		HuffmanTreeNode<weight>* root = hf.GetRoot(); // ����¹����Huffman���ĸ��ڵ�
		HuffmanTreeNode<weight>* cur = root; // ����һ��������������ָ��
		char ch = fgetc(fpress);  // ��ʼ��ѹ���ļ������ȡ����
		int pos = 8; // ÿ���ַ�һ���ֽڰ�λ����pos��Ӧ�ֽ�ÿ��λ�����㼴�ɻ���ֽڵ�ÿһλ��ֵ
		while (1)
		{
			--pos;
			if ((ch >> pos) & 1)  // ch����λ����ô������ҵĵ�һλ��ֵ���� 1 & �������һλ��ֵ��0����1�������1�Ļ������ҽڵ���
			{
				cur = cur->m_pRight;
			}
			else
			{
				cur = cur->m_pLeft; // ����� 0 �Ļ�������ڵ���
			}

			if (cur->m_pLeft == NULL && cur->m_pRight == NULL)  // ������ҽڵ㶼Ϊ�գ������Ѿ�����Ҷ�ӽ�㣬���Ѿ�����ȷ���������ĸ��ַ�����ӡ����ַ�Ȼ�����¿�ʼ
			{
				fputc(cur->m_tWeight.flag_ch, fUnCompress);
				cur = root;   //�ٴδӸ��ڵ����
				charcount--; // ��Ҫ��ӡ�����ַ�����1
			}
			if (pos == 0)  // ��λ����ֱ�Ӷ�ȡ��һ���ַ�
			{
				ch = fgetc(fpress);
				pos = 8;  // ����������һ����λflag
			}
			if (charcount == 0)  //����ȡѹ��ʱΪ�˴չ�һ���ֽڶ��ӽ�ȥ�ı���λ
				break;
		}

		fclose(fconfig);
		fclose(fpress);
		fclose(fUnCompress);
		return true;
	}

protected:
	bool _readLine(FILE* filename, string& line)   // ���ж�ȡ�ļ�
	{
		assert(filename);
		if (feof(filename))
			return false;
		unsigned char ch = fgetc(filename);

		while (ch != '\n')
		{
			line += ch;
			ch = fgetc(filename);

			if (feof(filename)) // �����һ���Ѿ������ļ�ĩβ��ֱ�ӷ���
				//break;
				return false;
		}
		return true;
	}

	void creat_all_codes(HuffmanTreeNode<weight>* root, string code) // ͨ��һ�Ź����õĹ����������ɱ���
	{
		if (NULL == root)
			return;
		if (root->m_pLeft == NULL && root->m_pRight == NULL)
		{
			weight_infos[root->m_tWeight.flag_ch].ch_code = code;

		}
		creat_all_codes(root->m_pLeft, code + '0'); // �ݹ����ɱ���
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