//*********************************************************
// CopyRight (C) 2015 Gooeen
// All rights reserved
//
// 文件名称 : mariadb.h
//
// 当前版本 : 1.0.1
// 作    者 : Gooeen
// 完成日期 : 2015/09/12
//
// 摘    要 : 用于访问 MySQL/MariaDB 的类库
//*********************************************************
#ifndef __SQL_MARIADB_H__
#define __SQL_MARIADB_H__

#include <mysql/mysql.h>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <tuple>
#include <memory>

// 如果指针是空则抛出异常
#define if_null_throw(pointer, message)\
if (pointer == nullptr)\
{\
	throw mariadb_exception(this->error() + "\r\nSQL: " + message, __FILE__, __LINE__);\
}

// 尝试读取数据, 如果没有数据则抛出异常
#define read_or_throw(reader, message)\
if (!reader.read())\
{\
	throw mariadb_exception(std::string() + "no data\r\nSQL: " + message, __FILE__, __LINE__);\
}

// 数据库操作
namespace sql
{
	// MySQL/MariaDB 操作
	namespace mariadb
	{
		class connection; // 数据库连接类
		class command; // 数据库执行类
		class recordset; // 数据库结果集类

		//*********************************************************
		// 函数名称 : to_date_string
		// 作    者 : Gooeen
		// 完成日期 : 2015/09/12
		// 函数说明 : 将日期转换成符合 MySQL/MariaDB 格式的字符串
		// 访问方式 : public
		// 函数参数 : unsigned int year 年份
		// 函数参数 : unsigned int month 月份
		// 函数参数 : unsigned int day 日期
		// 返 回 值 : std::string 符合 MySQL/MariaDB 格式的日期字符串
		// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
		//*********************************************************
		std::string to_date_string(unsigned int year, unsigned int month, unsigned int day);

		//*********************************************************
		// 函数名称 : to_time_string
		// 作    者 : Gooeen
		// 完成日期 : 2015/09/12
		// 函数说明 : 将时间转换成符合 MySQL/MariaDB 格式的字符串
		// 访问方式 : public
		// 函数参数 : unsigned int hour 时
		// 函数参数 : unsigned int minute 分
		// 函数参数 : unsigned int second 秒
		// 返 回 值 : std::string 符合 MySQL/MariaDB 格式的时间字符串
		// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
		//*********************************************************
		std::string to_time_string(unsigned int hour, unsigned int minute, unsigned int second);

		//*********************************************************
		// 函数名称 : to_datetime_string
		// 作    者 : Gooeen
		// 完成日期 : 2015/09/12
		// 函数说明 : 将日期时间转换成符合 MySQL/MariaDB 格式的字符串
		// 访问方式 : public
		// 函数参数 : unsigned int year 年
		// 函数参数 : unsigned int month 月
		// 函数参数 : unsigned int day 日
		// 函数参数 : unsigned int hour 时
		// 函数参数 : unsigned int minute 分
		// 函数参数 : unsigned int second 秒
		// 返 回 值 : std::string 符合 MySQL/MariaDB 格式的日期时间字符串
		// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
		//*********************************************************
		std::string to_datetime_string(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second);

		// MySQL/MariaDB 异常类
		class mariadb_exception : public std::exception
		{
		public:

			//*********************************************************
			// 函数名称 : mariadb_exception
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/12
			// 函数说明 : 构造函数
			// 访问方式 : public
			// 函数参数 : const std::string & text 异常信息
			// 函数参数 : const std::string & file 异常所在文件
			// 函数参数 : int line 异常所在行
			// 异    常 : 如果异常信息长度 (length) 超过字符串
			//            最大长度(max_size) 则抛出 std::length_error 异常;
			//            如果分配资源失败则抛出 std::bad_alloc 异常
			//*********************************************************
			mariadb_exception(const std::string &text, const std::string &file, int line);

			//*********************************************************
			// 函数名称 : what
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/12
			// 函数说明 : 输出异常信息
			// 访问方式 : public
			// 返 回 值 : const char * 异常信息字符串
			//*********************************************************
			const char * what() const override;

		private:
			std::string m_text; // 异常信息
		};

		// 数据库连接类
		// 一个数据库连接只能用于一个线程
		class connection
		{
		public:

			//*********************************************************
			// 函数名称 : connection
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 构造函数
			// 访问方式 : public
			//*********************************************************
			connection(void) noexcept;

			//*********************************************************
			// 函数名称 : connection
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 转移构造函数, 转移后原对象不应该使用
			// 访问方式 : public
			// 函数参数 : connection && connector 需要转移的对象
			//*********************************************************
			connection(connection &&connector) noexcept;

			//*********************************************************
			// 函数名称 : ~connection
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 如果数据库连接没有关闭则在析构时自动关闭
			// 访问方式 : public
			//*********************************************************
			~connection(void) noexcept;

			//*********************************************************
			// 函数名称 : operator!=
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 判断数据库连接器是否创建成功
			// 访问方式 : public
			// 函数参数 : std::nullptr_t null 与nullptr作比较
			// 返 回 值 : bool 如果创建成功则 对象 != nullptr;
			//            反之 对象 == nullptr
			//*********************************************************
			bool operator!=(std::nullptr_t null) const noexcept;

			//*********************************************************
			// 函数名称 : operator==
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 判断数据库连接器是否创建成功
			// 访问方式 : public
			// 函数参数 : std::nullptr_t null  与nullptr作比较
			// 返 回 值 : bool 如果创建成功则 对象 != nullptr;
			//            反之 对象 == nullptr
			//*********************************************************
			bool operator==(std::nullptr_t null) const noexcept;

			//*********************************************************
			// 函数名称 : open
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 连接数据库
			// 访问方式 : public
			// 函数参数 : const char * user 用户名
			// 函数参数 : const char * password 密码
			// 函数参数 : const char * database 数据库名
			// 函数参数 : unsigned int port 服务器端口
			// 函数参数 : const char * host 服务器 IP 地址
			// 函数参数 : const char * unix_socket 指定需要使用的 socket 或者 命名管道
			// 函数参数 : unsigned long flags 链接选项
			// 返 回 值 : bool true 代表连接成功, false 代表连接失败
			//*********************************************************
			bool open(const char *user, const char *password,
				const char *database, unsigned int port = 3306,
				const char *host = "127.0.0.1",
				const char *unix_socket = nullptr,
				unsigned long flags = 0) const noexcept;

			//*********************************************************
			// 函数名称 : close
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 关闭数据库连接, 如果不手动关闭数据库连接, 则析构时自动关闭
			// 访问方式 : public
			//*********************************************************
			void close(void) noexcept;

			//*********************************************************
			// 函数名称 : error
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据库操作失败的错误信息
			// 访问方式 : public
			// 返 回 值 : std::string 错误信息
			//*********************************************************
			std::string error(void) const noexcept;

			//*********************************************************
			// 函数名称 : errorno
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据库操作失败的错误代号
			// 访问方式 : public
			// 返 回 值 : unsigned int 错误代号
			//*********************************************************
			unsigned int errorno(void) const noexcept;

		private:

			//*********************************************************
			// 函数名称 : connection
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 禁止复制
			// 访问方式 : private 
			// 函数参数 : const connection &
			//*********************************************************
			connection(const connection &) = delete;

			//*********************************************************
			// 函数名称 : operator=
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 禁止复制
			// 访问方式 : private 
			// 函数参数 : const connection &
			// 返 回 值 : connection &
			//*********************************************************
			connection & operator=(const connection &) = delete;

		private:
			friend command;
			friend recordset;
			MYSQL *m_ptr_mysql; // MariaDB 数据库句柄
		};

		// 数据库结果集类
		class recordset
		{
		public:

			//*********************************************************
			// 函数名称 : recordset
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 转移构造函数, 转移后原对象不应该使用
			// 访问方式 : private
			// 函数参数 : recordset && reader 需要转移的对象
			//*********************************************************
			recordset(recordset &&reader) noexcept;

			//*********************************************************
			// 函数名称 : ~recordset
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 如果结果集没有释放关闭则在析构时自动关闭
			// 访问方式 : public
			//*********************************************************
			~recordset(void) noexcept;

			//*********************************************************
			// 函数名称 : operator!=
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 判断数据库连接器是否创建成功
			// 访问方式 : public
			// 函数参数 : std::nullptr_t null 与nullptr作比较
			// 返 回 值 : bool 如果创建成功则 对象 != nullptr;
			//            反之 对象 == nullptr
			//*********************************************************
			bool operator!=(std::nullptr_t null) const noexcept;

			//*********************************************************
			// 函数名称 : operator==
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 判断数据库连接器是否创建成功
			// 访问方式 : public
			// 函数参数 : std::nullptr_t null  与nullptr作比较
			// 返 回 值 : bool 如果创建成功则 对象 != nullptr;
			//            反之 对象 == nullptr
			//*********************************************************
			bool operator==(std::nullptr_t null) const noexcept;

			//*********************************************************
			// 函数名称 : close
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 释放结果集所占用的资源, 如果不手动释放资源, 则析构时自动释放
			// 访问方式 : public
			//*********************************************************
			void close(void) noexcept;

			//*********************************************************
			// 函数名称 : error
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据库操作失败的错误信息
			// 访问方式 : public
			// 返 回 值 : std::string 错误信息
			//*********************************************************
			std::string error(void) const noexcept;

			//*********************************************************
			// 函数名称 : errorno
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据库操作失败的错误代号
			// 访问方式 : public
			// 返 回 值 : unsigned int 错误代号
			//*********************************************************
			unsigned int errorno(void) const noexcept;

			//*********************************************************
			// 函数名称 : row_count
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据的行数
			// 访问方式 : public
			// 返 回 值 : unsigned long long 行数
			//*********************************************************
			unsigned long long row_count(void) const noexcept;

			//*********************************************************
			// 函数名称 : field_count
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据的列数(字段数量)
			// 访问方式 : public
			// 返 回 值 : unsigned long 列数(字段数量)
			//*********************************************************
			unsigned long field_count(void) const noexcept;

			//*********************************************************
			// 函数名称 : data_size
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取单条数据的长度
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置; 如果超出数据的列数则可能出现数组越界的问题
			// 返 回 值 : unsigned long 单条数据的字节数
			//*********************************************************
			unsigned long data_size(unsigned long n) const noexcept;

			//*********************************************************
			// 函数名称 : read
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 读取下一条数据
			// 访问方式 : public
			// 返 回 值 : bool 成功返回true, 失败返回false
			//*********************************************************
			bool read(void) noexcept;

			//*********************************************************
			// 函数名称 : get_char
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据强制转换成 char 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : char 数据
			//*********************************************************
			char get_char(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_udata
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据用 std::vector<unsigned char> 对象保存
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : std::vector<unsigned char> 数据
			// 异    常 : 如果 std::vector 创建失败时抛出异常
			//*********************************************************
			std::vector<unsigned char> get_udata(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_udata_ptr
			// 作    者 : Gooeen
			// 完成日期 : 2016/05/09
			// 函数说明 : 获取数据, 并将数据用 std::vector<unsigned char> 对象保存
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : std::vector<unsigned char> 数据
			// 异    常 : 如果 std::vector 创建失败时抛出异常
			//*********************************************************
			std::shared_ptr<std::vector<unsigned char>> get_udata_ptr(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_data
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据用 std::vector<char> 对象保存
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : std::vector<char> 数据
			// 异    常 : 如果 std::vector 创建失败时抛出异常
			//*********************************************************
			std::vector<char> get_data(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_data_ptr
			// 作    者 : Gooeen
			// 完成日期 : 2016/05/09
			// 函数说明 : 获取数据, 并将数据用 std::vector<char> 对象保存
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : std::vector<char> 数据
			// 异    常 : 如果 std::vector 创建失败时抛出异常
			//*********************************************************
			std::shared_ptr<std::vector<char>> get_data_ptr(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_double
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据转换成 double 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : double 数据
			//*********************************************************
			double get_double(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_float
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据转换成 float 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : float 数据
			//*********************************************************
			float get_float(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_int
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据转换成 int 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : int 数据
			//*********************************************************
			int get_int(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_long
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据转换成 long 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : long 数据
			//*********************************************************
			long get_long(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_longlong
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据转换成 long long 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : long long 数据
			//*********************************************************
			long long get_longlong(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_short
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据转换成 short 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : short 数据
			//*********************************************************
			short get_short(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_string
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : std::string 数据
			// 异    常 : 如果数据长度 (length) 超过字符串最大长度(max_size)
			//            则抛出 std::length_error 异常;
			//            如果分配资源失败则抛出 std::bad_alloc 异常
			//*********************************************************
			std::string get_string(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_string_ptr
			// 作    者 : Gooeen
			// 完成日期 : 2016/05/09
			// 函数说明 : 获取数据
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : std::string 数据
			// 异    常 : 如果数据长度 (length) 超过字符串最大长度(max_size)
			//            则抛出 std::length_error 异常;
			//            如果分配资源失败则抛出 std::bad_alloc 异常
			//*********************************************************
			std::shared_ptr<std::string> get_string_ptr(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_uchar
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据强制转换成 unsigned char 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : unsigned char 数据
			//*********************************************************
			unsigned char get_uchar(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_uint
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据转换成 unsigned int 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : unsigned int 数据
			// 异    常 : 如果强制转换失败, 则抛出 std::invalid_argument 异常;
			//            如果数据值大于 unsigned int 的取值范围, 则抛出 std::out_of_range 异常
			//*********************************************************
			unsigned int get_uint(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_ulong
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 获取数据, 并将数据转换成 unsigned long 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : unsigned long 数据
			// 异    常 : 如果强制转换失败, 则抛出 std::invalid_argument 异常;
			//            如果数据值大于 unsigned long 的取值范围, 则抛出 std::out_of_range 异常
			//*********************************************************
			unsigned long get_ulong(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_ulonglong
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 获取数据, 并将数据转换成 unsigned long long 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : unsigned long long 数据
			// 异    常 : 如果强制转换失败, 则抛出 std::invalid_argument 异常;
			//            如果数据值大于 unsigned long long 的取值范围, 则抛出 std::out_of_range 异常
			//*********************************************************
			unsigned long long get_ulonglong(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_ushort
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 获取数据, 并将数据转换成 unsigned short 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : unsigned short 数据
			//*********************************************************
			unsigned short get_ushort(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_wchar
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 获取数据, 并将数据强制转换成 wchar_t 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : wchar_t 数据
			//*********************************************************
			wchar_t get_wchar(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_wstring
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 获取数据, 并将数据强制转换成 std::wstring 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : std::wstring 数据
			//*********************************************************
			std::wstring get_wstring(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_wstring_ptr
			// 作    者 : Gooeen
			// 完成日期 : 2015/05/09
			// 函数说明 : 获取数据, 并将数据强制转换成 std::wstring 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : std::shared_ptr<std::wstring> 数据
			//*********************************************************
			std::shared_ptr<std::wstring> get_wstring_ptr(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_raw
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 不进行转换, 直接获取从数据库返回的数据
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : const char * 数据
			//*********************************************************
			const char * get_raw(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_bool
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 获取数据, 并将数据强制转换成 bool 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : bool 数据
			//*********************************************************
			bool get_bool(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置, 从0开始
			// 返 回 值 : T 数据
			//*********************************************************
			template <typename T>
			T get(unsigned long n) const;

			//*********************************************************
			// 函数名称 : get_char
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据强制转换成 char 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : char 数据
			//*********************************************************
			template <>
			char get<char>(unsigned long n) const
			{
				return get_char(n);
			}

			//*********************************************************
			// 函数名称 : get_double
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据转换成 double 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : double 数据
			//*********************************************************
			template <>
			double get<double>(unsigned long n) const
			{
				return get_double(n);
			}

			//*********************************************************
			// 函数名称 : get_float
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据转换成 float 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : float 数据
			//*********************************************************
			template <>
			float get<float>(unsigned long n) const
			{
				return get_float(n);
			}

			//*********************************************************
			// 函数名称 : get_int
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据转换成 int 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : int 数据
			//*********************************************************
			template <>
			int get<int>(unsigned long n) const
			{
				return get_int(n);
			}

			//*********************************************************
			// 函数名称 : get_longlong
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据转换成 long long 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : long long 数据
			//*********************************************************
			template <>
			long long get<long long>(unsigned long n) const
			{
				return get_longlong(n);
			}

			//*********************************************************
			// 函数名称 : get_long
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据转换成 long 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : long 数据
			//*********************************************************
			template <>
			long get<long>(unsigned long n) const
			{
				return get_long(n);
			}

			//*********************************************************
			// 函数名称 : get_short
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据转换成 short 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : short 数据
			//*********************************************************
			template <>
			short get<short>(unsigned long n) const
			{
				return get_short(n);
			}

			//*********************************************************
			// 函数名称 : get_string
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : std::string 数据
			// 异    常 : 如果数据长度 (length) 超过字符串最大长度(max_size)
			//            则抛出 std::length_error 异常;
			//            如果分配资源失败则抛出 std::bad_alloc 异常
			//*********************************************************
			template <>
			std::string get<std::string>(unsigned long n) const
			{
				return get_string(n);
			}

			//*********************************************************
			// 函数名称 : get_string_ptr
			// 作    者 : Gooeen
			// 完成日期 : 2016/05/09
			// 函数说明 : 获取数据
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : std::string 数据
			// 异    常 : 如果数据长度 (length) 超过字符串最大长度(max_size)
			//            则抛出 std::length_error 异常;
			//            如果分配资源失败则抛出 std::bad_alloc 异常
			//*********************************************************
			template <>
			std::shared_ptr<std::string> get<std::shared_ptr<std::string>>(unsigned long n) const
			{
				return get_string_ptr(n);
			}

			//*********************************************************
			// 函数名称 : get_udata
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据用 std::vector<unsigned char> 对象保存
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : std::vector<unsigned char> 数据
			// 异    常 : 如果 std::vector 创建失败时抛出异常
			//*********************************************************
			template <>
			std::vector<unsigned char> get<std::vector<unsigned char>>(unsigned long n) const
			{
				return get_udata(n);
			}

			//*********************************************************
			// 函数名称 : get_udata_ptr
			// 作    者 : Gooeen
			// 完成日期 : 2016/05/09
			// 函数说明 : 获取数据, 并将数据用 std::vector<unsigned char> 对象保存
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : std::vector<unsigned char> 数据
			// 异    常 : 如果 std::vector 创建失败时抛出异常
			//*********************************************************
			template <>
			std::shared_ptr<std::vector<unsigned char>> get<std::shared_ptr<std::vector<unsigned char>>>(unsigned long n) const
			{
				return get_udata_ptr(n);
			}

			//*********************************************************
			// 函数名称 : get_data
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据用 std::vector<char> 对象保存
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : std::vector<char> 数据
			// 异    常 : 如果 std::vector 创建失败时抛出异常
			//*********************************************************
			template <>
			std::vector<char> get<std::vector<char>>(unsigned long n) const
			{
				return get_data(n);
			}

			//*********************************************************
			// 函数名称 : get_data_ptr
			// 作    者 : Gooeen
			// 完成日期 : 2016/05/09
			// 函数说明 : 获取数据, 并将数据用 std::vector<char> 对象保存
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : std::vector<char> 数据
			// 异    常 : 如果 std::vector 创建失败时抛出异常
			//*********************************************************
			template <>
			std::shared_ptr<std::vector<char>> get<std::shared_ptr<std::vector<char>>>(unsigned long n) const
			{
				return get_data_ptr(n);
			}

			//*********************************************************
			// 函数名称 : get_wstring
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 获取数据, 并将数据强制转换成 std::wstring 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : std::wstring 数据
			//*********************************************************
			template <>
			std::wstring get<std::wstring>(unsigned long n) const
			{
				return get_wstring(n);
			}

			//*********************************************************
			// 函数名称 : get_wstring_ptr
			// 作    者 : Gooeen
			// 完成日期 : 2015/05/09
			// 函数说明 : 获取数据, 并将数据强制转换成 std::wstring 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : std::shared_ptr<std::wstring> 数据
			//*********************************************************
			template <>
			std::shared_ptr<std::wstring> get<std::shared_ptr<std::wstring>>(unsigned long n) const
			{
				return get_wstring_ptr(n);
			}

			//*********************************************************
			// 函数名称 : get_uchar
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据强制转换成 unsigned char 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : unsigned char 数据
			//*********************************************************
			template <>
			unsigned char get<unsigned char>(unsigned long n) const
			{
				return get_uchar(n);
			}

			//*********************************************************
			// 函数名称 : get_uint
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/13
			// 函数说明 : 获取数据, 并将数据转换成 unsigned int 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : unsigned int 数据
			// 异    常 : 如果强制转换失败, 则抛出 std::invalid_argument 异常;
			//            如果数据值大于 unsigned int 的取值范围, 则抛出 std::out_of_range 异常
			//*********************************************************
			template <>
			unsigned int get<unsigned int>(unsigned long n) const
			{
				return get_uint(n);
			}

			//*********************************************************
			// 函数名称 : get_ulonglong
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 获取数据, 并将数据转换成 unsigned long long 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : unsigned long long 数据
			// 异    常 : 如果强制转换失败, 则抛出 std::invalid_argument 异常;
			//            如果数据值大于 unsigned long long 的取值范围, 则抛出 std::out_of_range 异常
			//*********************************************************
			template <>
			unsigned long long get<unsigned long long>(unsigned long n) const
			{
				return get_ulonglong(n);
			}

			//*********************************************************
			// 函数名称 : get_ulong
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 获取数据, 并将数据转换成 unsigned long 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : unsigned long 数据
			// 异    常 : 如果强制转换失败, 则抛出 std::invalid_argument 异常;
			//            如果数据值大于 unsigned long 的取值范围, 则抛出 std::out_of_range 异常
			//*********************************************************
			template <>
			unsigned long get<unsigned long>(unsigned long n) const
			{
				return get_ulong(n);
			}

			//*********************************************************
			// 函数名称 : get_ushort
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 获取数据, 并将数据转换成 unsigned short 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : unsigned short 数据
			//*********************************************************
			template <>
			unsigned short get<unsigned short>(unsigned long n) const
			{
				return get_ushort(n);
			}

			//*********************************************************
			// 函数名称 : get_wchar
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 获取数据, 并将数据强制转换成 wchar_t 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : wchar_t 数据
			//*********************************************************
			template <>
			wchar_t get<wchar_t>(unsigned long n) const
			{
				return get_wchar(n);
			}

			//*********************************************************
			// 函数名称 : get_raw
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 不进行转换, 直接获取从数据库返回的数据
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : const char * 数据
			//*********************************************************
			template <>
			const char * get<const char *>(unsigned long n) const
			{
				return get_raw(n);
			}

			//*********************************************************
			// 函数名称 : get_bool
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 获取数据, 并将数据强制转换成 bool 类型
			// 访问方式 : public
			// 函数参数 : unsigned long n 数据的列位置
			// 返 回 值 : bool 数据
			//*********************************************************
			template <>
			bool get<bool>(unsigned long n) const
			{
				return get_bool(n);
			}

		private:

			//*********************************************************
			// 函数名称 : recordset
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 由 command 类对象创建 recordset 类对象
			// 访问方式 : private
			// 函数参数 : MYSQL * pointer 数据库句柄
			//*********************************************************
			recordset(MYSQL *pointer) noexcept;

			//*********************************************************
			// 函数名称 : recordset
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 禁止复制
			// 访问方式 : private
			// 函数参数 : const recordset &
			//*********************************************************
			recordset(const recordset &) = delete;

			//*********************************************************
			// 函数名称 : operator=
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 禁止复制
			// 访问方式 : private
			// 函数参数 : const recordset &
			// 返 回 值 : recordset &
			//*********************************************************
			recordset & operator=(const recordset &) = delete;

		private:
			friend command;
			MYSQL *m_ptr_mysql; // MariaDB 数据库句柄
			MYSQL_RES *m_ptr_res; // MariaDB 数据库结果集
			MYSQL_ROW m_row; // MariaDB 数据库结果行
		};

		// 数据库执行类
		// 一个 command 对象只能用于一个线程
		// 同一个线程下, 可以多个 command 对象共享一个 数据库连接 connection 对象
		class command
		{
		public:

			//*********************************************************
			// 函数名称 : command
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 转移构造函数, 转移后原对象不应该使用
			// 访问方式 : public
			// 函数参数 : command && executor 需要转移的对象
			//*********************************************************
			command(command &&executor) noexcept;

			//*********************************************************
			// 函数名称 : command
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 构造函数
			// 访问方式 : public
			// 函数参数 : const connection & connector 数据库连接类
			//*********************************************************
			explicit command(const connection &connector) noexcept;

			//*********************************************************
			// 函数名称 : command
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 构造函数
			// 访问方式 : public
			// 函数参数 : const connection & connector 数据库连接类
			// 函数参数 : const std::string & text SQL语句
			// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
			//*********************************************************
			command(const connection &connector, const std::string &text);

			//*********************************************************
			// 函数名称 : command
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 构造函数
			// 访问方式 : public
			// 函数参数 : const connection & connector 数据库连接类
			// 函数参数 : std::string && text SQL语句
			//*********************************************************
			command(const connection &connector, std::string &&text) noexcept;

			//*********************************************************
			// 函数名称 : error
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 获取数据库操作失败的错误信息
			// 访问方式 : public 
			// 函数参数 : void
			// 返 回 值 : std::string 错误信息
			//*********************************************************
			std::string error(void) const noexcept;

			//*********************************************************
			// 函数名称 : errorno
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 获取数据库操作失败的错误代号
			// 访问方式 : public
			// 返 回 值 : unsigned int 错误代号
			//*********************************************************
			unsigned int errorno(void) const noexcept;

			//*********************************************************
			// 函数名称 : prepare
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 预存SQL语句; 可使用以下例子防注入
			//            executer.prepare("select * from table where id = ?");
			//            executer.add(12);
			// 访问方式 : public
			// 函数参数 : std::string && text SQL语句
			//*********************************************************
			void prepare(std::string &&text) noexcept;

			//*********************************************************
			// 函数名称 : prepare
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/15
			// 函数说明 : 预存SQL语句; 可使用以下例子防注入
			//            executer.prepare("select * from table1 where id = ?");
			//            executer.add(0, 12);
			// 访问方式 : public
			// 函数参数 : const std::string & text SQL语句
			// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
			//*********************************************************
			void prepare(const std::string &text);

			//*********************************************************
			// 函数名称 : add
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 添加数据, 执行SQL语句时将本次添加的数据用单引
			//            号引起后代替SQL语句中第 pos 个问号; 例子如下:
			//            executer.prepare("insert into table1 values(?, ?)");
			//            executer.add(0, 12);
			//            executer.add(1, "data");
			//            在执行SQL语句时将产生 SQL语句:
			//            insert into table1 values(12, 'data')
			// 访问方式 : public
			// 函数参数 : unsigned int pos 需要代替问号的位置, 从0开始
			// 函数参数 : const std::string & text 数据, 此数据在SQL语句中将以单引号引用
			// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
			//*********************************************************
			void add(unsigned int pos, const std::string &text);

			//*********************************************************
			// 函数名称 : add
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 添加数据, 执行SQL语句时将本次添加的数据用单引
			//            号引起后代替SQL语句中第 pos 个问号; 例子如下:
			//            executer.prepare("insert into table1 values(?, ?)");
			//            executer.add(0, 12);
			//            executer.add(1, "data");
			//            在执行SQL语句时将产生 SQL语句:
			//            insert into table1 values(12, 'data')
			// 访问方式 : public
			// 函数参数 : unsigned int pos 需要代替问号的位置, 从0开始
			// 函数参数 : const std::string & text 数据, 此数据在SQL语句中将以单引号引用
			// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
			//*********************************************************
			void add(unsigned int pos, std::string &&text);

			//*********************************************************
			// 函数名称 : add
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 添加数据, 执行SQL语句时将本次添加的数据用单引
			//            号引起后代替SQL语句中第 pos 个问号; 例子如下:
			//            executer.prepare("insert into table1 values(?, ?)");
			//            executer.add(0, 12);
			//            executer.add(1, "data");
			//            在执行SQL语句时将产生 SQL语句:
			//            insert into table1 values(12, 'data')
			// 访问方式 : public
			// 函数参数 : unsigned int pos 需要代替问号的位置, 从0开始
			// 函数参数 : const std::string & text 数据, 此数据在SQL语句中将以单引号引用
			// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
			//*********************************************************
			void add(unsigned int pos, const std::vector<char> &data);

			//*********************************************************
			// 函数名称 : add
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 添加数据, 执行SQL语句时将本次添加的数据用单引
			//            号引起后代替SQL语句中第 pos 个问号; 例子如下:
			//            executer.prepare("insert into table1 values(?, ?)");
			//            executer.add(0, 12);
			//            executer.add(1, "data");
			//            在执行SQL语句时将产生 SQL语句:
			//            insert into table1 values(12, 'data')
			// 访问方式 : public
			// 函数参数 : unsigned int pos 需要代替问号的位置, 从0开始
			// 函数参数 : const std::string & text 数据, 此数据在SQL语句中将以单引号引用
			// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
			//*********************************************************
			void add(unsigned int pos, std::vector<char> &&data);

			//*********************************************************
			// 函数名称 : add
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 添加数据, 执行SQL语句时将本次添加的数据用单引
			//            号引起后代替SQL语句中第 pos 个问号; 例子如下:
			//            executer.prepare("insert into table1 values(?, ?)");
			//            executer.add(0, 12);
			//            executer.add(1, "data");
			//            在执行SQL语句时将产生 SQL语句:
			//            insert into table1 values(12, 'data')
			// 访问方式 : public
			// 函数参数 : unsigned int pos 需要代替问号的位置, 从0开始
			// 函数参数 : const std::string & text 数据, 此数据在SQL语句中将以单引号引用
			// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
			//*********************************************************
			void add(unsigned int pos, const std::vector<unsigned char> &data);

			//*********************************************************
			// 函数名称 : add
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 添加数据, 执行SQL语句时将本次添加的数据用单引
			//            号引起后代替SQL语句中第 pos 个问号; 例子如下:
			//            executer.prepare("insert into table1 values(?, ?)");
			//            executer.add(0, 12);
			//            executer.add(1, "data");
			//            在执行SQL语句时将产生 SQL语句:
			//            insert into table1 values(12, 'data')
			// 访问方式 : public
			// 函数参数 : unsigned int pos 需要代替问号的位置, 从0开始
			// 函数参数 : const std::string & text 数据, 此数据在SQL语句中将以单引号引用
			// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
			//*********************************************************
			void add(unsigned int pos, std::vector<unsigned char> &&data);

			//*********************************************************
			// 函数名称 : add
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 添加数据, 执行SQL语句时将本次添加的数据
			//            代替SQL语句中第 pos 个问号; 例子如下:
			//            executer.prepare("insert into table1 values(?, ?)");
			//            executer.add(0, 12);
			//            executer.add(1, "data");
			//            在执行SQL语句时将产生 SQL语句:
			//            insert into table1 values(12, 'data')
			// 访问方式 : public
			// 函数参数 : unsigned int pos 需要代替问号的位置, 从0开始
			// 函数参数 : const std::string & text 数据, 此数据在SQL语句中将以单引号引用
			// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
			//*********************************************************
			template <typename T>
			void add(unsigned int pos, T value)
			{
				// 保存字符串
				m_strings.push_back(std::to_string(value));

				// 保存数据的信息
				const auto p = m_strings.back().c_str();
				const auto size = m_strings.back().size();
				const auto pair = std::make_pair(p, size);
				m_parameters[pos] = std::make_pair(true, pair);
			}

			//*********************************************************
			// 函数名称 : escape
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 转换数据并保存到 to 参数所指向的缓冲区
			// 访问方式 : public
			// 函数参数 : char * to 保存转换后的字符串的缓冲区, 缓冲区大小应该比 (size * 2 + 1) 大
			// 函数参数 : const char * from 需要转换的字符串
			// 函数参数 : unsigned long size from 的长度
			// 返 回 值 : unsigned long 转换的字节数
			//*********************************************************
			unsigned long escape(char *to, const char *from, unsigned long size) const noexcept;

			//*********************************************************
			// 函数名称 : escape
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 转换数据并保存到字符串对象中
			// 访问方式 : public
			// 函数参数 : const char * data 需要转换的数据
			// 函数参数 : unsigned long size 需要转换的数据的长度
			// 返 回 值 : std::string 转换后的字符串
			// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常;
			//            如果执行失败则抛出 std::exception 异常
			//*********************************************************
			std::string escape(const char *data, unsigned long size) const;

			//*********************************************************
			// 函数名称 : escape_with_quote
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 转换数据并以单引号引用保存到字符串对象中
			// 访问方式 : public
			// 函数参数 : const char * data 需要转换的数据
			// 函数参数 : unsigned long size 需要转换的数据的长度
			// 返 回 值 : std::string 转换后的字符串
			// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常;
			//            如果执行失败则抛出 std::exception 异常
			//*********************************************************
			std::string escape_with_quote(const char *data, unsigned long size) const;

			//*********************************************************
			// 函数名称 : escape_buffer
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 转换数据并保存到 std::vector<char> 类型的缓冲区中
			// 访问方式 : public
			// 函数参数 : const char * data 需要转换的数据
			// 函数参数 : unsigned long size 需要转换的数据的长度
			// 返 回 值 : std::vector<char> 保存转换后的数据的缓冲区
			// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常;
			//            如果执行失败则抛出 std::exception 异常
			//*********************************************************
			std::vector<char> escape_buffer(const char *data, unsigned long size) const;

			//*********************************************************
			// 函数名称 : escape_buffer_with_quote
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 转换数据并并以单引号引用保存到 std::vector<char> 类型的缓冲区中
			// 访问方式 : public
			// 函数参数 : const char * data 需要转换的数据
			// 函数参数 : unsigned long size 需要转换的数据的长度
			// 返 回 值 : std::vector<char> 保存转换后的数据的缓冲区
			// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常;
			//            如果执行失败则抛出 std::exception 异常
			//*********************************************************
			std::vector<char> escape_buffer_with_quote(const char *data, unsigned long size) const;

			//*********************************************************
			// 函数名称 : execute
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句
			// 访问方式 : public
			// 函数参数 : const char * text SQL语句
			// 返 回 值 : bool 如果SQL语句执行成功返回true; 反之返回false
			//*********************************************************
			bool execute(const char *text) const noexcept;

			//*********************************************************
			// 函数名称 : execute
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句
			// 访问方式 : public
			// 函数参数 : const char * text SQL语句
			// 函数参数 : unsigned long length SQL语句字符串长度
			// 返 回 值 : bool 如果SQL语句执行成功返回true; 反之返回false
			//*********************************************************
			bool execute(const char *text, unsigned long length) const noexcept;

			//*********************************************************
			// 函数名称 : execute
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句
			// 访问方式 : public
			// 函数参数 : const std::string & text SQL语句
			// 返 回 值 : bool 如果SQL语句执行成功返回true; 反之返回false
			//*********************************************************
			bool execute(const std::string &text) const noexcept;

			//*********************************************************
			// 函数名称 : execute
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句
			// 访问方式 : public
			// 函数参数 : const std::vector<char> & data SQL语句
			// 返 回 值 : bool 如果SQL语句执行成功返回true; 反之返回false
			//*********************************************************
			bool execute(const std::vector<char> &data) const noexcept;

			//*********************************************************
			// 函数名称 : execute
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句
			// 访问方式 : public
			// 返 回 值 : bool 如果SQL语句执行成功返回true; 反之返回false
			// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常;
			//            如果生成SQL语句失败则抛出 std::exception 异常
			//*********************************************************
			bool execute(void) const;

			//*********************************************************
			// 函数名称 : execute
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句; 使用例子:
			//            command(connector).execute("insert into table1 values(?, ?)", std::make_tuple(12, "abc"));
			//            执行上面语句将生成SQL语句: insert into table1 values(12, 'abc')
			//            生成SQL语句后执行SQL语句
			// 访问方式 : public
			// 函数参数 : const char * text SQL语句
			// 函数参数 : const Tuple & t 数据
			// 返 回 值 : bool 如果SQL语句执行成功返回true; 反之返回false
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename Tuple>
			bool execute(const std::string &text, const Tuple &t) const
			{
				return this->execute(statement<Tuple>().generate(*this, text, t));
			}

			//*********************************************************
			// 函数名称 : execute
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句
			// 访问方式 : public 
			// 函数参数 : const std::vector<char> & data SQL语句
			// 函数参数 : const Tuple & t 数据
			// 返 回 值 : bool 如果SQL语句执行成功返回true; 反之返回false
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename Tuple>
			bool execute(const std::vector<char> &data, const Tuple &t) const
			{
				return this->execute(statement<Tuple>().generate(*this, data, t));
			}

			//*********************************************************
			// 函数名称 : execute_reader
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句
			// 访问方式 : public
			// 函数参数 : const char * text SQL语句
			// 返 回 值 : sql::mariadb::recordset 结果集
			// 异    常 : 如果执行SQL失败则抛出 mariadb_exception 异常
			//*********************************************************
			recordset execute_reader(const char *text) const;

			//*********************************************************
			// 函数名称 : execute_reader
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句
			// 访问方式 : public
			// 函数参数 : const char * text SQL语句
			// 函数参数 : unsigned long length text的字节数
			// 返 回 值 : sql::mariadb::recordset 结果集
			// 异    常 : 如果执行SQL失败则抛出 mariadb_exception 异常
			//*********************************************************
			recordset execute_reader(const char *text, unsigned long length) const;

			//*********************************************************
			// 函数名称 : execute_reader
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句
			// 访问方式 : public
			// 函数参数 : const std::string & text SQL语句
			// 返 回 值 : sql::mariadb::recordset 结果集
			// 异    常 : 如果执行SQL失败则抛出 mariadb_exception 异常
			//*********************************************************
			recordset execute_reader(const std::string &text) const;

			//*********************************************************
			// 函数名称 : execute_reader
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句
			// 访问方式 : public
			// 函数参数 : const std::vector<char> & data SQL语句
			// 返 回 值 : sql::mariadb::recordset 结果集
			// 异    常 : 如果执行SQL失败则抛出 mariadb_exception 异常
			//*********************************************************
			recordset execute_reader(const std::vector<char> &data) const;

			//*********************************************************
			// 函数名称 : execute_reader
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句
			// 访问方式 : public
			// 返 回 值 : sql::mariadb::recordset 结果集
			// 异    常 : 如果执行SQL失败则抛出 mariadb_exception 异常
			//            如果分配资源失败则抛出 std::bad_alloc 异常;
			//            如果生成SQL语句失败则抛出 std::exception 异常
			//*********************************************************
			recordset execute_reader(void) const;

			//*********************************************************
			// 函数名称 : execute_reader
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句; 使用例子:
			//            command(connector).execute_reader("insert into table1 values(?, ?)", std::make_tuple(12, "abc"));
			//            执行上面语句将生成SQL语句: insert into table1 values(12, 'abc')
			//            生成SQL语句后执行SQL语句
			// 访问方式 : public
			// 函数参数 : const std::string & text SQL语句
			// 函数参数 : const Tuple & t 数据
			// 返 回 值 : sql::mariadb::recordset 结果集
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename Tuple>
			recordset execute_reader(const std::string &text, const Tuple &t) const
			{
				return this->execute_reader(statement<Tuple>().generate(*this, text, t));
			}

			//*********************************************************
			// 函数名称 : execute_reader
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句
			// 访问方式 : public 
			// 函数参数 : const std::vector<char> & data SQL语句
			// 函数参数 : const Tuple & t 数据
			// 返 回 值 : sql::mariadb::recordset 结果集
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename Tuple>
			recordset execute_reader(const std::vector<char> &data, const Tuple &t) const
			{
				return this->execute_reader(statement<Tuple>().generate(*this, data, t));
			}

			//*********************************************************
			// 函数名称 : execute_scalar
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回第一列的第一个值
			// 访问方式 : public
			// 函数参数 : const char * text SQL语句
			// 返 回 值 : T 结果集中第一列的第一个值
			// 异    常 : 如果执行SQL失败或者不能获取结果集或者转换失败则抛出 mariadb_exception 异常
			//*********************************************************
			template <typename T>
			T execute_scalar(const char *text) const
			{
				auto reader = this->execute_reader(text);
				if_null_throw(reader, text);
				read_or_throw(reader, text);
				return reader.get<T>(0);
			}

			//*********************************************************
			// 函数名称 : execute_scalar
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回第一列的第一个值
			// 访问方式 : public
			// 函数参数 : const char * text SQL语句
			// 函数参数 : unsigned long length text的字节数
			// 返 回 值 : T 结果集中第一列的第一个值
			// 异    常 : 如果执行SQL失败或者不能获取结果集或者转换失败则抛出 mariadb_exception 异常
			//*********************************************************
			template <typename T>
			T execute_scalar(const char *text, unsigned long length) const
			{
				auto reader = this->execute_reader(text, length);
				if_null_throw(reader, std::string(text, length));
				read_or_throw(reader, std::string(text, length));
				return reader.get<T>(0);
			}

			//*********************************************************
			// 函数名称 : execute_scalar
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回第一列的第一个值
			// 访问方式 : public
			// 函数参数 : const std::string & text SQL语句
			// 返 回 值 : T 结果集中第一列的第一个值
			// 异    常 : 如果执行SQL失败或者不能获取结果集或者转换失败则抛出 mariadb_exception 异常
			//*********************************************************
			template <typename T>
			T execute_scalar(const std::string &text) const
			{
				auto reader = this->execute_reader(text);
				if_null_throw(reader, text);
				read_or_throw(reader, text);
				return reader.get<T>(0);
			}

			//*********************************************************
			// 函数名称 : execute_scalar
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回第一列的第一个值
			// 访问方式 : public
			// 函数参数 : const std::vector<char> & data SQL语句
			// 返 回 值 : T 结果集中第一列的第一个值
			// 异    常 : 如果执行SQL失败或者不能获取结果集或者转换失败则抛出 mariadb_exception 异常
			//*********************************************************
			template <typename T>
			T execute_scalar(const std::vector<char> &data) const
			{
				auto reader = this->execute_reader(data);
				if_null_throw(reader, std::string(data.begin(), data.end()));
				read_or_throw(reader, std::string(data.begin(), data.end()));
				return reader.get<T>(0);
			}

			//*********************************************************
			// 函数名称 : execute_scalar
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回第一列的第一个值
			// 访问方式 : public
			// 返 回 值 : T 结果集中第一列的第一个值
			// 异    常 : 如果执行SQL失败或者不能获取结果集或者转换失败则抛出 mariadb_exception 异常
			//*********************************************************
			template <typename T>
			T execute_scalar(void) const
			{
				auto reader = this->execute_reader();
				if_null_throw(reader, m_text);
				read_or_throw(reader, m_text);
				return reader.get<T>(0);
			}

			//*********************************************************
			// 函数名称 : execute_scalar
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回第一列的第一个值
			// 访问方式 : public
			// 函数参数 : const std::string & text SQL语句
			// 函数参数 : const Tuple & t 数据
			// 返 回 值 : T 结果集中第一列的第一个值
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename T, typename Tuple>
			T execute_scalar(const std::string &text, const Tuple &t) const
			{
				return this->execute_scalar<T>(statement<Tuple>().generate(*this, text, t));
			}

			//*********************************************************
			// 函数名称 : execute_scalar
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回第一列的第一个值
			// 访问方式 : public
			// 函数参数 : const std::vector<char> & data SQL语句
			// 函数参数 : const Tuple & t 数据
			// 返 回 值 : T 结果集中第一列的第一个值
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename T, typename Tuple>
			T execute_scalar(const std::vector<char> &data, const Tuple &t) const
			{
				return this->execute_scalar<T>(statement<Tuple>().generate(*this, data, t));
			}

			//*********************************************************
			// 函数名称 : query
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回第一行数据
			// 访问方式 : public
			// 函数参数 : const char * text SQL语句
			// 返 回 值 : Tuple 结果集中第一行数据
			// 异    常 : 如果执行SQL失败或者不能获取结果集或者转换失败则抛出 mariadb_exception 异常
			//*********************************************************
			template <typename Tuple>
			Tuple query(const char *text) const
			{
				// 获取结果集, 如果获取失败则抛出异常
				auto reader = this->execute_reader(text);
				if_null_throw(reader, text);
				read_or_throw(reader, text);
				return data_tuple<Tuple>::get(reader);
			}

			//*********************************************************
			// 函数名称 : query
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回第一行数据
			// 访问方式 : public
			// 函数参数 : const char * text SQL语句
			// 函数参数 : unsigned long length text的字节数
			// 返 回 值 : Tuple 结果集中第一行数据
			// 异    常 : 如果执行SQL失败或者不能获取结果集或者转换失败则抛出 mariadb_exception 异常
			//*********************************************************
			template <typename Tuple>
			Tuple query(const char *text, unsigned long length) const
			{
				// 获取结果集, 如果获取失败则抛出异常
				auto reader = this->execute_reader(text, length);
				if_null_throw(reader, std::string(text, length));
				read_or_throw(reader, std::string(text, length));
				return data_tuple<Tuple>::get(reader);
			}

			//*********************************************************
			// 函数名称 : query
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回第一行数据
			// 访问方式 : public
			// 函数参数 : const std::string & text SQL语句
			// 返 回 值 : Tuple 结果集中第一行数据
			// 异    常 : 如果执行SQL失败或者不能获取结果集或者转换失败则抛出 mariadb_exception 异常
			//*********************************************************
			template <typename Tuple>
			Tuple query(const std::string &text) const
			{
				// 获取结果集, 如果获取失败则抛出异常
				auto reader = this->execute_reader(text);
				if_null_throw(reader, text);
				read_or_throw(reader, text);
				return data_tuple<Tuple>::get(reader);
			}

			//*********************************************************
			// 函数名称 : query
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回第一行数据
			// 访问方式 : public 
			// 函数参数 : const std::vector<char> & data SQL语句
			// 返 回 值 : Tuple 结果集中第一行数据
			// 异    常 : 如果执行SQL失败或者不能获取结果集或者转换失败则抛出 mariadb_exception 异常
			//*********************************************************
			template <typename Tuple>
			Tuple query(const std::vector<char> &data) const
			{
				// 获取结果集, 如果获取失败则抛出异常
				auto reader = this->execute_reader(data);
				if_null_throw(reader, std::string(data.begin(), data.end()));
				read_or_throw(reader, std::string(data.begin(), data.end()));
				return data_tuple<Tuple>::get(reader);
			}

			//*********************************************************
			// 函数名称 : query
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回第一行数据
			// 访问方式 : public
			// 返 回 值 : Tuple 结果集中第一行数据
			// 异    常 :  如果执行SQL失败或者不能获取结果集或者转换失败则抛出 mariadb_exception 异常
			//*********************************************************
			template <typename Tuple>
			Tuple query(void) const
			{
				// 获取结果集, 如果获取失败则抛出异常
				auto reader = this->execute_reader();
				if_null_throw(reader, m_text);
				read_or_throw(reader, m_text);
				return data_tuple<Tuple>::get(reader);
			}

			//*********************************************************
			// 函数名称 : query
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回第一行数据
			// 访问方式 : public
			// 函数参数 : const std::string & text SQL语句
			// 函数参数 : const InTuple & t 数据
			// 返 回 值 : OutTuple 结果集中第一行数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename OutTuple, typename InTuple>
			OutTuple query(const std::string &text, const InTuple &t) const
			{
				return this->query<OutTuple>(statement<InTuple>().generate(*this, text, t));
			}

			//*********************************************************
			// 函数名称 : query
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回第一行数据
			// 访问方式 : public
			// 函数参数 : const std::vector<char> & data SQL语句
			// 函数参数 : const InTuple & t 数据
			// 返 回 值 : OutTuple 结果集中第一行数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename OutTuple, typename InTuple>
			OutTuple query(const std::vector<char> &data, const InTuple &t) const
			{
				return this->query<OutTuple>(statement<InTuple>().generate(*this, data, t));
			}

			//*********************************************************
			// 函数名称 : query_vector
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回所有数据
			// 访问方式 : public
			// 函数参数 : const char * text SQL语句
			// 返 回 值 : std::vector<Tuple> 结果集中所有数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename Tuple>
			std::vector<Tuple> query_vector(const char *text) const
			{
				// 获取结果集, 如果获取失败则抛出异常
				auto reader = this->execute_reader(text);
				if_null_throw(reader, text);
				return vector_from_recordset<Tuple>(reader);
			}

			//*********************************************************
			// 函数名称 : query_vector
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回所有数据
			// 访问方式 : public 
			// 函数参数 : const char * text SQL语句
			// 函数参数 : unsigned long length text的字节数
			// 返 回 值 : std::vector<Tuple> 结果集中所有数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename Tuple>
			std::vector<Tuple> query_vector(const char *text, unsigned long length) const
			{
				// 获取结果集, 如果获取失败则抛出异常
				auto reader = this->execute_reader(text, length);
				if_null_throw(reader, std::string(text, length));
				return vector_from_recordset<Tuple>(reader);
			}

			//*********************************************************
			// 函数名称 : query_vector
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回所有数据
			// 访问方式 : public 
			// 函数参数 : const std::string & text SQL语句
			// 返 回 值 : std::vector<Tuple> 结果集中所有数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename Tuple>
			std::vector<Tuple> query_vector(const std::string &text) const
			{
				// 获取结果集, 如果获取失败则抛出异常
				auto reader = this->execute_reader(text);
				if_null_throw(reader, text);
				return vector_from_recordset<Tuple>(reader);
			}

			//*********************************************************
			// 函数名称 : query_vector
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回所有数据
			// 访问方式 : public
			// 函数参数 : const std::vector<char> & data SQL语句
			// 返 回 值 : std::vector<Tuple> 结果集中所有数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename Tuple>
			std::vector<Tuple> query_vector(const std::vector<char> &data) const
			{
				// 获取结果集, 如果获取失败则抛出异常
				auto reader = this->execute_reader(data);
				if_null_throw(reader, std::string(data.begin(), data.end()));
				return vector_from_recordset<Tuple>(reader);
			}

			//*********************************************************
			// 函数名称 : query_vector
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 执行SQL语句并返回所有数据
			// 访问方式 : public
			// 返 回 值 : std::vector<Tuple> 结果集中所有数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename Tuple>
			std::vector<Tuple> query_vector(void) const
			{
				// 获取结果集, 如果获取失败则抛出异常
				auto reader = this->execute_reader();
				if_null_throw(reader, m_text);
				return vector_from_recordset<Tuple>(reader);
			}

			//*********************************************************
			// 函数名称 : query_vector
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/17
			// 函数说明 : 执行SQL语句并返回所有数据
			// 访问方式 : public
			// 函数参数 : const std::string & text SQL语句
			// 函数参数 : const InTuple & t 数据
			// 返 回 值 : std::vector<Tuple> 结果集中所有数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename OutTuple, typename InTuple>
			std::vector<OutTuple> query_vector(const std::string &text, const InTuple &t) const
			{
				return this->query_vector<OutTuple>(statement<InTuple>().generate(*this, text, t));
			}

			//*********************************************************
			// 函数名称 : query_vector
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/17
			// 函数说明 : 执行SQL语句并返回所有数据
			// 访问方式 : public
			// 函数参数 : const std::vector<char> & data SQL语句
			// 函数参数 : const InTuple & t 数据
			// 返 回 值 : std::vector<Tuple> 结果集中所有数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename OutTuple, typename InTuple>
			std::vector<OutTuple> query_vector(const std::vector<char> &data, const InTuple &t) const
			{
				return this->query_vector<OutTuple>(statement<InTuple>().generate(*this, data, t));
			}

			//*********************************************************
			// 函数名称 : query_list
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/17
			// 函数说明 : 执行SQL语句并返回所有数据
			// 访问方式 : public
			// 函数参数 : const char * text SQL语句
			// 返 回 值 : std::list<Tuple> 结果集中所有数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename Tuple>
			std::list<Tuple> query_list(const char *text) const
			{
				// 获取结果集, 如果获取失败则抛出异常
				auto reader = this->execute_reader(text);
				if_null_throw(reader, text);
				return list_from_recordset<Tuple>(reader);
			}

			//*********************************************************
			// 函数名称 : query_list
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/17
			// 函数说明 : 执行SQL语句并返回所有数据
			// 访问方式 : public
			// 函数参数 : const char * text SQL语句
			// 函数参数 : unsigned long length text的字节数
			// 返 回 值 : std::list<Tuple> 结果集中所有数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename Tuple>
			std::list<Tuple> query_list(const char *text, unsigned long length) const
			{
				// 获取结果集, 如果获取失败则抛出异常
				auto reader = this->execute_reader(text, length);
				if_null_throw(reader, std::string(text, length));
				return list_from_recordset<Tuple>(reader);
			}

			//*********************************************************
			// 函数名称 : query_list
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/17
			// 函数说明 : 执行SQL语句并返回所有数据
			// 访问方式 : public
			// 函数参数 : const std::string & text SQL语句
			// 返 回 值 : std::list<Tuple> 结果集中所有数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename Tuple>
			std::list<Tuple> query_list(const std::string &text) const
			{
				// 获取结果集, 如果获取失败则抛出异常
				auto reader = this->execute_reader(text);
				if_null_throw(reader, text);
				return list_from_recordset<Tuple>(reader);
			}

			//*********************************************************
			// 函数名称 : query_list
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/17
			// 函数说明 : 执行SQL语句并返回所有数据
			// 访问方式 : public
			// 函数参数 : const std::vector<char> & data SQL语句
			// 返 回 值 : std::list<Tuple> 结果集中所有数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename Tuple>
			std::list<Tuple> query_list(const std::vector<char> &data) const
			{
				// 获取结果集, 如果获取失败则抛出异常
				auto reader = this->execute_reader(data);
				if_null_throw(reader, std::string(data.begin(), data.end()));
				return list_from_recordset<Tuple>(reader);
			}

			//*********************************************************
			// 函数名称 : query_list
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/17
			// 函数说明 : 执行SQL语句并返回所有数据
			// 访问方式 : public
			// 返 回 值 : std::list<Tuple> 结果集中所有数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename Tuple>
			std::list<Tuple> query_list(void) const
			{
				// 获取结果集, 如果获取失败则抛出异常
				auto reader = this->execute_reader();
				if_null_throw(reader, m_text);
				return list_from_recordset<Tuple>(reader);
			}

			//*********************************************************
			// 函数名称 : query_list
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/17
			// 函数说明 : 执行SQL语句并返回所有数据
			// 访问方式 : public 
			// 函数参数 : const std::string & text SQL语句
			// 函数参数 : const InTuple & t 数据
			// 返 回 值 : std::list<Tuple> 结果集中所有数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename OutTuple, typename InTuple>
			std::list<OutTuple> query_list(const std::string &text, const InTuple &t) const
			{
				return this->query_list<OutTuple>(statement<InTuple>().generate(*this, text, t));
			}

			//*********************************************************
			// 函数名称 : query_list
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/17
			// 函数说明 : 执行SQL语句并返回所有数据
			// 访问方式 : public 
			// 函数参数 : const std::vector<char> & data SQL语句
			// 函数参数 : const InTuple & t 数据
			// 返 回 值 : std::list<Tuple> 结果集中所有数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename OutTuple, typename InTuple>
			std::list<OutTuple> query_list(const std::vector<char> &data, const InTuple &t) const
			{
				return this->query_list<OutTuple>(statement<InTuple>().generate(*this, data, t));
			}

		private:

			//*********************************************************
			// 函数名称 : vector_from_recordset
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 从结果集中读取数据并填充到 std::vector 对象中返回
			// 访问方式 : private
			// 函数参数 : recordset & reader 结果集
			// 返 回 值 : std::vector<Tuple> 结果集中的所有数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename Tuple>
			static std::vector<Tuple> vector_from_recordset(recordset &reader)
			{
				// 分配内存
				std::vector<Tuple> data; // 保存数据
				data.reserve((unsigned int)reader.row_count());

				// 赋值
				while (reader.read())
				{
					data.push_back(data_tuple<Tuple>::get(reader));
				}
				return data;
			}

			//*********************************************************
			// 函数名称 : list_from_recordset
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 从结果集中读取数据并填充到 std::list 对象中返回
			// 访问方式 : private
			// 函数参数 : recordset & reader 结果集
			// 返 回 值 : std::list<Tuple> 结果集中的所有数据
			// 异    常 : 如果执行失败则抛出异常
			//*********************************************************
			template <typename Tuple>
			static std::list<Tuple> list_from_recordset(recordset &reader)
			{
				// 赋值
				std::list<Tuple> data; // 保存数据
				while (reader.read())
				{
					data.push_back(data_tuple<Tuple>::get(reader));
				}
				return data;
			}

			// 用于将结果集中读取到的数据保存到 tuple 对象中并返回
			template <typename Tuple>
			struct data_tuple
			{
				//*********************************************************
				// 函数名称 : get
				// 作    者 : Gooeen
				// 完成日期 : 2015/09/16
				// 函数说明 : 返回从结果集中读取到的数据, 并用 tuple 对象保存
				// 访问方式 : public
				// 函数参数 : const recordset & reader 结果集对象
				// 返 回 值 : Tuple 从结果集中读取到的数据
				// 异    常 : 如果执行失败则抛出异常
				//*********************************************************
				static Tuple get(const recordset &reader)
				{
					Tuple t;
					set<std::tuple_size<Tuple>::value>(t, reader);
					return t;
				}

				//*********************************************************
				// 函数名称 : set
				// 作    者 : Gooeen
				// 完成日期 : 2015/09/16
				// 函数说明 : 将结果集中读取到的数据保存到指定的参数 t 中
				// 模板参数 : size_t Size 函数参数 t 的元素数量
				// 访问方式 : public
				// 函数参数 : Tuple & t 用于将从结果集中读取到的数据填充到该参数指定位置
				// 函数参数 : const recordset & reader 结果集对象
				// 异    常 : 如果执行失败则抛出异常
				//*********************************************************
				template <size_t Size>
				static void set(Tuple &t, const recordset &reader)
				{
					const auto n = Size - 1;
					std::get<n>(t) = reader.get<std::tuple_element<n, Tuple>::type>(n);
					set<n>(t, reader);
				}

				//*********************************************************
				// 函数名称 : set
				// 作    者 : Gooeen
				// 完成日期 : 2015/09/16
				// 函数说明 : 将结果集中读取到的数据保存到指定的参数 t 中
				// 模板参数 : size_t Size 函数参数 t 的元素数量
				// 访问方式 : public
				// 函数参数 : Tuple & t 用于将从结果集中读取到的数据填充到该参数指定位置
				// 函数参数 : const recordset & reader 结果集对象
				// 异    常 : 如果执行失败则抛出异常
				//*********************************************************
				template <>
				static void set<1>(Tuple &t, const recordset &reader)
				{
					std::get<0>(t) = reader.get<std::tuple_element<0, Tuple>::type>(0);
				}
			};

			// 根据提供的带问号的SQL语句和 tuple 对象生成完整的SQL语句
			// tuple 的元素类型只能是 数 和 std::string 型字符串
			// 和 std::vector<char> 型和 std::vector<unsigned char> 型缓冲区
			template <typename Tuple>
			class statement
			{
			private:

				//*********************************************************
				// 函数名称 : escape_buffer
				// 作    者 : Gooeen
				// 完成日期 : 2015/09/16
				// 函数说明 : 将数转换成字符串并保存到 std::vector<char> 对象中
				// 访问方式 : public
				// 函数参数 : const command & command 对象
				// 函数参数 : Type type 数
				// 返 回 值 : std::vector<char> 字符串形式的数字
				// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常;
				//            如果执行失败则抛出 std::exception 异常
				//*********************************************************
				template <typename Type>
				static std::vector<char> escape_buffer(const command &, Type type)
				{
					const auto text = std::to_string(type);
					return std::vector<char>(text.begin(), text.end());
				}

				//*********************************************************
				// 函数名称 : escape_buffer
				// 作    者 : Gooeen
				// 完成日期 : 2015/09/16
				// 函数说明 : 转换数据
				// 访问方式 : public
				// 函数参数 : const command & executer command 对象
				// 函数参数 : const std::string & data 字符串
				// 返 回 值 : std::vector<char> 字符串形式的数字
				// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常;
				//            如果执行失败则抛出 std::exception 异常
				//*********************************************************
				static std::vector<char> escape_buffer(const command &executer, const std::string &data)
				{
					return executer.escape_buffer_with_quote(data.c_str(), data.size());
				}

				//*********************************************************
				// 函数名称 : escape_buffer
				// 作    者 : Gooeen
				// 完成日期 : 2015/09/16
				// 函数说明 : 转换数据
				// 访问方式 : public
				// 函数参数 : const command & executer command 对象
				// 函数参数 : const std::vector<char> & data 数据
				// 返 回 值 : std::vector<char> 字符串形式的数字
				// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常;
				//            如果执行失败则抛出 std::exception 异常
				//*********************************************************
				static std::vector<char> escape_buffer(const command &executer, const std::vector<char> &data)
				{
					return executer.escape_buffer_with_quote(data.data(), data.size());
				}

				//*********************************************************
				// 函数名称 : escape_buffer
				// 作    者 : Gooeen
				// 完成日期 : 2015/09/16
				// 函数说明 : 转换数据
				// 访问方式 : public
				// 函数参数 : const command & executer command 对象
				// 函数参数 : const std::vector<unsigned char> & data 数据
				// 返 回 值 : std::vector<char> 字符串形式的数字
				// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常;
				//            如果执行失败则抛出 std::exception 异常
				//*********************************************************
				static std::vector<char> escape_buffer(const command &executer, const std::vector<unsigned char> &data)
				{
					return executer.escape_buffer_with_quote((const char *)data.data(), data.size());
				}

				//*********************************************************
				// 函数名称 : escape
				// 作    者 : Gooeen
				// 完成日期 : 2015/09/16
				// 函数说明 : 将 t 所保存的数据进行转化后保存到 list 对象中
				// 访问方式 : public
				// 函数参数 : const command & executer command 对象
				// 函数参数 : const Tuple & t 数据
				// 返 回 值 : std::list<std::vector<char>> 转化后的数据
				// 异    常 : 如果执行失败则抛出 std::exception 异常
				//*********************************************************
				template <size_t size>
				std::list<std::vector<char>> escape(const command &executer, const Tuple &t)
				{
					// 保存数据
					std::list<std::vector<char>> datas = escape<size - 1>(executer, t);
					datas.push_back(escape_buffer(executer, std::get<size - 1>(t)));

					// 添加字节数
					m_size += datas.back().size();

					return datas;
				}

				//*********************************************************
				// 函数名称 : escape
				// 作    者 : Gooeen
				// 完成日期 : 2015/09/16
				// 函数说明 : 将 t 所保存的数据进行转化后保存到 list 对象中
				// 访问方式 : public
				// 函数参数 : const command & executer command 对象
				// 函数参数 : const Tuple & t 数据
				// 返 回 值 : std::list<std::vector<char>> 转化后的数据
				// 异    常 : 如果执行失败则抛出 std::exception 异常
				//*********************************************************
				template <>
				std::list<std::vector<char>> escape<1>(const command &executer, const Tuple &t)
				{
					// 保存数据
					std::list<std::vector<char>> datas;
					datas.push_back(escape_buffer(executer, std::get<0>(t)));

					// 添加字节数
					m_size += datas.back().size();

					return datas;
				}

			public:

				//*********************************************************
				// 函数名称 : generate
				// 作    者 : Gooeen
				// 完成日期 : 2015/09/16
				// 函数说明 : 生成SQL语句并保存到 std::vector<char> 对象中
				// 访问方式 : public
				// 函数参数 : const command & executer command 对象
				// 函数参数 : const Type & text SQL语句, std::string 或者 std::vector<char>
				// 函数参数 : const Tuple & t 数据
				// 返 回 值 : std::vector<char> 完整的SQL语句
				// 异    常 : 如果执行失败则抛出 std::exception 异常
				//*********************************************************
				template <typename Type>
				std::vector<char> generate(const command &executer, const Type &text, const Tuple &t)
				{
					// 重置最终完整的SQL语句的字节数
					m_size = text.size();

					// 减去问号数量
					m_size -= std::count(text.begin(), text.end(), '?');

					// 获取转换后的数据并且计算最终完整的SQL语句的字节数
					std::list<std::vector<char>> datas = this->escape<std::tuple_size<Tuple>::value>(executer, t);

					// 用于保存最终完整的SQL语句
					std::vector<char> data(m_size);

					// 用于赋值操作
					auto vect_iter = data.begin();
					auto list_iter = datas.begin();

					// 生成SQL语句
					for (auto ch : text)
					{
						// 如果不等于问号则直接复制字符
						if (ch != '?')
						{
							*vect_iter = ch;
							++vect_iter;
						}
						// 如果等于问号则复制数据
						else
						{
							const auto p = data.data() + (vect_iter - data.begin());
							memcpy(p, list_iter->data(), list_iter->size());
							vect_iter += list_iter->size();
							++list_iter;
						}
					}

					return data;
				}

			private:
				size_t m_size; // 最终完整的SQL语句的字节数
			};

		private:

			//*********************************************************
			// 函数名称 : command
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 禁止复制
			// 访问方式 : private
			// 函数参数 : const command &
			//*********************************************************
			command(const command &) = delete;

			//*********************************************************
			// 函数名称 : operator=
			// 作    者 : Gooeen
			// 完成日期 : 2015/09/16
			// 函数说明 : 禁止复制
			// 访问方式 : private
			// 函数参数 : const command &
			// 返 回 值 : command &
			//*********************************************************
			command & operator=(const command &) = delete;

		private:
			friend recordset;
			typedef std::pair<const char *, unsigned long> byte_data; // 数据开始位置和大小
			typedef std::pair<bool, byte_data> alnum_data; // 是否一个数和数据

			MYSQL *m_ptr_mysql; // MariaDB 数据库句柄
			std::string m_text; // SQL语句
			std::list<std::string> m_strings; // 保存字符串数据
			std::list<std::vector<char>> m_datas; // 保存数据
			std::list<std::vector<unsigned char>> m_udatas; // 保存数据
			std::map<unsigned int, alnum_data> m_parameters; // 保存SQL语句的数据
		};
	}
}

#undef if_null_throw
#undef read_or_throw

#endif // __SQL_MARIADB_H__

