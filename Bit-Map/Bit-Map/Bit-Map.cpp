// Bit-Map.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <fstream>
#include "bitmap.h"
#include "bloomfilter.h"
using namespace std;

void bitmap_bloomfilter_number_test();
void bloomfilter_search_url();
void read_file(string file_name);
int main(int argc, char ** argv)
{
	//bitmap_bloomfilter_number_test();
	/*read_file("../file1.txt");*/
	bloomfilter_search_url();
	return(0);
}

//位图算法和布隆过滤器的数值存储效率比较
void bitmap_bloomfilter_number_test() {
	//生成一万个随机数(训练数据结构)，1000个测试随机数，范围0-100万
	int train_number_count = 10000;
	int max_num = 1000000;
	vector<int> train_random_numbers;
	train_random_numbers.resize(train_number_count);
	srand((unsigned)time(NULL));
	for (int i = 0; i < train_number_count; ++i) {
		train_random_numbers[i] = rand() % (max_num + 1);
	}
	cout << "训练随机数个数：" << train_random_numbers.size() << endl;


	//BitMap的范围直接指定0-1000000,并将训练数据加入其中
	BitMap bitmap(0, 1000000);
	for (int i = 0; i < train_random_numbers.size(); ++i) {
		bitmap.set(train_random_numbers[i]);
	}
	//布隆过滤器的设置和使用
	bloom_parameters parameters;
	//插入数据个数,10000个训练数据
	parameters.projected_element_count = 10000;
	//期望误判率0.0001
	parameters.false_positive_probability = 0.0001;
	//随机种子
	parameters.random_seed = 0xA5A5A5A5;
	//判断参数合法性
	if (!parameters)
	{
		cout << "Error - Invalid set of bloom filter parameters!" << endl;
		return;
	}
	//计算其余参数
	parameters.compute_optimal_parameters();
	//初始化布隆过滤器
	bloom_filter filter(parameters);
	//插入到布隆过滤器中，10000个随机数
	for (int i = 0; i < train_number_count; ++i) {
		filter.insert(train_random_numbers[i]);
	}

	//生成测试数据
	int test_number_count = 1000;
	vector<int> test_random_numbers;
	test_random_numbers.resize(test_number_count);
	/*srand((unsigned)time(NULL));*/
	for (int i = 0; i < test_number_count; ++i) {
		test_random_numbers[i] = rand() % (max_num + 1);
	}
	cout << "测试随机数个数：" << test_random_numbers.size() << endl;

	//测试
	int count_not_in_bitmap = 0;
	int count_not_in_filter = 0;
	cout << "测试结果：" << endl;
	for (int i = 0; i < test_number_count; ++i) {
		bool flag_bitmap = bitmap.test(test_random_numbers[i]);
		if (!flag_bitmap)count_not_in_bitmap++;
		bool flag_filter = filter.contains(test_random_numbers[i]);
		if (!flag_filter)count_not_in_filter++;
	}
	cout << "data" << "\t" << "bitmap" << "\t" << "filter" << endl;
	cout << "in" << "\t" << test_number_count - count_not_in_bitmap << "\t" << test_number_count - count_not_in_filter << endl;
	cout << "not in" << "\t" << count_not_in_bitmap << "\t" << count_not_in_filter << endl;
	cout << "误判率：" << (float)(count_not_in_bitmap - count_not_in_filter) / test_number_count << endl;
}

//布隆过滤器挑选两个文件中重复的url
void bloomfilter_search_url() {
	//读取文件
	string file_name1 = "../file1.txt", file_name2 = "../file2.txt",out_filename="../same_url";
	vector<string> file1_url, file2_url;
	ifstream infile_1, infile_2;
	ofstream outfile;
	string s;
	infile_1.open(file_name1);
	while (infile_1 >> s) {
		file1_url.push_back(s);
	}
	infile_1.close();
	infile_2.open(file_name2);
	while (infile_2 >> s) {
		file2_url.push_back(s);
	}
	infile_2.close();

	//比较文件行数,插入文件行数较小的
	int min_file = 1;
	if (file1_url.size() > file2_url.size()) min_file = 2;
	cout << "文件1行数："<<file1_url.size() <<"\t"<<"文件2行数:"<< file2_url.size() <<"\t"
		<<"文件行数较小的文件："<< min_file << endl;

	//初始化布隆过滤器
	//布隆过滤器的设置和使用
	bloom_parameters parameters;
	//插入数据个数,较小的文件的行数
	if (min_file == 1) parameters.projected_element_count = file1_url.size();
	else parameters.projected_element_count = file2_url.size();
	//期望误判率0.0001
	parameters.false_positive_probability = 0.0001;
	//随机种子
	parameters.random_seed = 0xA5A5A5A5;
	//判断参数合法性
	if (!parameters)
	{
		cout << "Error - Invalid set of bloom filter parameters!" << endl;
		return;
	}
	//计算其余参数
	parameters.compute_optimal_parameters();
	//初始化布隆过滤器
	bloom_filter filter(parameters);
	//插入到布隆过滤器中，较小文件中的url
	for (int i = 0; i < file1_url.size(); ++i) {
		filter.insert(file1_url[i]);
	}
	//检查另一个文件中的url,是否有相同的，有则输出
	outfile.open(out_filename);
	for (int i = 0; i < file2_url.size(); ++i) {
		if (filter.contains(file2_url[i])) {
			outfile << file2_url[i] << endl;
			cout << file2_url[i] << endl;
		}
	}
}
//读取文件
void read_file(string file_name) {
	ifstream infile_1;
	string s;
	infile_1.open(file_name);
	for (int i = 0; i < 10; ++i) {
		infile_1 >> s;
		cout << s << endl;
	}
	//infile_2.open("file2.txt");
	while (true) {

	}
	infile_1.close();
}
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
