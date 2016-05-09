//*********************************************************
// CopyRight (C) 2015 Gooeen
// All rights reserved
//
// 文件名称 : mariadb.cpp
//
// 当前版本 : 1.0.1
// 作    者 : Gooeen
// 完成日期 : 2015/09/12
//
// 摘    要 : 用于访问 MySQL/MariaDB 的类库
//*********************************************************
#include "mariadb.h"
#include <cassert>


// 如果条件是 false 则抛出异常
#define if_false_throw(condition, message)\
if (!condition)\
{\
	throw mariadb_exception(this->error() + "\r\nSQL: " + message, __FILE__, __LINE__);\
}


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
// 异    常 : 如果尝试分配空间失败则抛出 std::bad_alloc 异常
//*********************************************************
std::string sql::mariadb::to_date_string(unsigned int year, unsigned int month, unsigned int day)
{
	const auto y = std::to_string(year);
	const auto m = std::to_string(month);
	const auto d = std::to_string(day);
	return y + "-" + m + "-" + d;
}


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
// 异    常 : 如果尝试分配空间失败则抛出 std::bad_alloc 异常
//*********************************************************
std::string sql::mariadb::to_time_string(unsigned int hour, unsigned int minute, unsigned int second)
{
	const auto h = std::to_string(hour);
	const auto m = std::to_string(minute);
	const auto s = std::to_string(second);
	return h + "-" + m + "-" + s;
}


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
// 异    常 : 如果尝试分配空间失败则抛出 std::bad_alloc 异常
//*********************************************************
std::string sql::mariadb::to_datetime_string(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second)
{
	const auto d = to_date_string(year, month, day);
	const auto t = to_time_string(hour, minute, second);
	return d + " " + t;
}


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
//            如果尝试分配空间失败则抛出 std::bad_alloc 异常
//*********************************************************
sql::mariadb::mariadb_exception::mariadb_exception(const std::string & text, const std::string & file, int line)
	: m_text(text + "\r\nin " + file + ", row: " + std::to_string(line))
{
}


//*********************************************************
// 函数名称 : what
// 作    者 : Gooeen
// 完成日期 : 2015/09/12
// 函数说明 : 输出异常信息
// 访问方式 : public
// 返 回 值 : const char * 异常信息字符串
//*********************************************************
const char * sql::mariadb::mariadb_exception::what() const
{
	return m_text.c_str();
}


//*********************************************************
// 函数名称 : connection
// 作    者 : Gooeen
// 完成日期 : 2015/09/13
// 函数说明 : 构造函数
// 访问方式 : public
//*********************************************************
sql::mariadb::connection::connection(void) noexcept
	: m_ptr_mysql(mysql_init(nullptr))
{
}


//*********************************************************
// 函数名称 : connection
// 作    者 : Gooeen
// 完成日期 : 2015/09/13
// 函数说明 : 转移构造函数, 转移后原对象不应该使用
// 访问方式 : public
// 函数参数 : connection && connector 需要转移的对象
//*********************************************************
sql::mariadb::connection::connection(connection && connector) noexcept
	: m_ptr_mysql(connector.m_ptr_mysql)
{
	connector.m_ptr_mysql = nullptr;
}


//*********************************************************
// 函数名称 : ~connection
// 作    者 : Gooeen
// 完成日期 : 2015/09/13
// 函数说明 : 如果数据库连接没有关闭则在析构时自动关闭
// 访问方式 : public
//*********************************************************
sql::mariadb::connection::~connection(void) noexcept
{
	if (m_ptr_mysql != nullptr)
	{
		mysql_close(m_ptr_mysql);
	}
}


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
bool sql::mariadb::connection::operator!=(std::nullptr_t null) const noexcept
{
	return m_ptr_mysql != null;
}


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
bool sql::mariadb::connection::operator==(std::nullptr_t null) const noexcept
{
	return m_ptr_mysql == null;
}


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
bool sql::mariadb::connection::open(const char * user, const char * password, const char * database, unsigned int port, const char * host, const char * unix_socket, unsigned long flags) const noexcept
{
	assert(host != nullptr);
	assert(user != nullptr);
	assert(password != nullptr);

	if (m_ptr_mysql == nullptr)
	{
		return false;
	}

	bool result = mysql_real_connect(m_ptr_mysql, host, user, password, database, port, unix_socket, flags) != nullptr;
	mysql_set_character_set(m_ptr_mysql, "utf8");

	return result;
}


//*********************************************************
// 函数名称 : close
// 作    者 : Gooeen
// 完成日期 : 2015/09/13
// 函数说明 : 关闭数据库连接, 如果不关闭数据库连接, 则析构时自动关闭
// 访问方式 : public
//*********************************************************
void sql::mariadb::connection::close(void) noexcept
{
	if (m_ptr_mysql != nullptr)
	{
		mysql_close(m_ptr_mysql);
		m_ptr_mysql = nullptr;
	}
}


//*********************************************************
// 函数名称 : error
// 作    者 : Gooeen
// 完成日期 : 2015/09/13
// 函数说明 : 获取数据库操作失败的错误信息
// 访问方式 : public
// 返 回 值 : std::string 错误信息
//*********************************************************
std::string sql::mariadb::connection::error(void) const noexcept
try
{
	return std::string(mysql_error(m_ptr_mysql));
}
catch (const std::exception &)
{
	return std::string();
}


//*********************************************************
// 函数名称 : errorno
// 作    者 : Gooeen
// 完成日期 : 2015/09/13
// 函数说明 : 获取数据库操作失败的错误代号
// 访问方式 : public
// 返 回 值 : unsigned int 错误代号
//*********************************************************
unsigned int sql::mariadb::connection::errorno(void) const noexcept
{
	return mysql_errno(m_ptr_mysql);
}


//*********************************************************
// 函数名称 : recordset
// 作    者 : Gooeen
// 完成日期 : 2015/09/13
// 函数说明 : 转移构造函数, 转移后原对象不应该使用
// 访问方式 : private
// 函数参数 : recordset && reader 需要转移的对象
//*********************************************************
sql::mariadb::recordset::recordset(recordset && reader) noexcept
	: m_ptr_mysql(reader.m_ptr_mysql)
	, m_ptr_res(reader.m_ptr_res)
	, m_row(reader.m_row)
{
	reader.m_ptr_mysql = nullptr;
	reader.m_ptr_res = nullptr;
	reader.m_row = nullptr;
}


//*********************************************************
// 函数名称 : ~recordset
// 作    者 : Gooeen
// 完成日期 : 2015/09/13
// 函数说明 : 如果结果集没有释放关闭则在析构时自动关闭
// 访问方式 : public
//*********************************************************
sql::mariadb::recordset::~recordset(void) noexcept
{
	if (m_ptr_res != nullptr)
	{
		mysql_free_result(m_ptr_res);
	}
}


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
bool sql::mariadb::recordset::operator!=(std::nullptr_t null) const noexcept
{
	return m_ptr_res != null;
}


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
bool sql::mariadb::recordset::operator==(std::nullptr_t null) const noexcept
{
	return m_ptr_res == null;
}


//*********************************************************
// 函数名称 : close
// 作    者 : Gooeen
// 完成日期 : 2015/09/13
// 函数说明 : 释放结果集所占用的资源, 如果不手动释放资源, 则析构时自动释放
// 访问方式 : public
//*********************************************************
void sql::mariadb::recordset::close(void) noexcept
{
	if (m_ptr_res != nullptr)
	{
		mysql_free_result(m_ptr_res);
		m_ptr_res = nullptr;
		m_ptr_mysql = nullptr;
	}
}


//*********************************************************
// 函数名称 : error
// 作    者 : Gooeen
// 完成日期 : 2015/09/13
// 函数说明 : 获取数据库操作失败的错误信息
// 访问方式 : public
// 返 回 值 : std::string 错误信息
//*********************************************************
std::string sql::mariadb::recordset::error(void) const noexcept
try
{
	return std::string(mysql_error(m_ptr_mysql));
}
catch (const std::exception &)
{
	return std::string();
}


//*********************************************************
// 函数名称 : errorno
// 作    者 : Gooeen
// 完成日期 : 2015/09/13
// 函数说明 : 获取数据库操作失败的错误代号
// 访问方式 : public
// 返 回 值 : unsigned int 错误代号
//*********************************************************
unsigned int sql::mariadb::recordset::errorno(void) const noexcept
{
	return mysql_errno(m_ptr_mysql);
}


//*********************************************************
// 函数名称 : row_count
// 作    者 : Gooeen
// 完成日期 : 2015/09/13
// 函数说明 : 获取数据的行数
// 访问方式 : public
// 返 回 值 : unsigned long long 行数
//*********************************************************
unsigned long long sql::mariadb::recordset::row_count(void) const noexcept
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	return mysql_num_rows(m_ptr_res);
}


//*********************************************************
// 函数名称 : field_count
// 作    者 : Gooeen
// 完成日期 : 2015/09/13
// 函数说明 : 获取数据的列数(字段数量)
// 访问方式 : public
// 返 回 值 : unsigned long 列数(字段数量)
//*********************************************************
unsigned long sql::mariadb::recordset::field_count(void) const noexcept
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	return mysql_num_fields(m_ptr_res);
}


//*********************************************************
// 函数名称 : data_size
// 作    者 : Gooeen
// 完成日期 : 2015/09/13
// 函数说明 : 获取单条数据的长度
// 访问方式 : public
// 函数参数 : unsigned long n 数据的列位置; 如果超出数据的列数则可能出现数组越界的问题
// 返 回 值 : unsigned long 单条数据的字节数
//*********************************************************
unsigned long sql::mariadb::recordset::data_size(unsigned long n) const noexcept
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return mysql_fetch_lengths(m_ptr_res)[n];
}


//*********************************************************
// 函数名称 : read
// 作    者 : Gooeen
// 完成日期 : 2015/09/13
// 函数说明 : 读取下一条数据
// 访问方式 : public
// 返 回 值 : bool 成功返回true, 失败返回false
//*********************************************************
bool sql::mariadb::recordset::read(void) noexcept
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	m_row = mysql_fetch_row(m_ptr_res);
	return m_row != nullptr;
}


//*********************************************************
// 函数名称 : get_char
// 作    者 : Gooeen
// 完成日期 : 2015/09/13
// 函数说明 : 获取数据, 并将数据强制转换成 char 类型
// 访问方式 : public
// 函数参数 : unsigned long n 数据的列位置
// 返 回 值 : char 数据
//*********************************************************
char sql::mariadb::recordset::get_char(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return m_row[n][0];
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
std::vector<unsigned char> sql::mariadb::recordset::get_udata(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return std::vector<unsigned char>(m_row[n], m_row[n] + this->data_size(n));
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
std::vector<char> sql::mariadb::recordset::get_data(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return std::vector<char>(m_row[n], m_row[n] + this->data_size(n));
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
double sql::mariadb::recordset::get_double(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return std::atof(m_row[n]);
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
float sql::mariadb::recordset::get_float(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return (float)std::atof(m_row[n]);
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
int sql::mariadb::recordset::get_int(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return std::atoi(m_row[n]);
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
long sql::mariadb::recordset::get_long(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return std::atol(m_row[n]);
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
long long sql::mariadb::recordset::get_longlong(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return std::atoll(m_row[n]);
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
short sql::mariadb::recordset::get_short(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return (short)std::atoi(m_row[n]);
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
//            如果尝试分配空间失败则抛出 std::bad_alloc 异常
//*********************************************************
std::string sql::mariadb::recordset::get_string(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return std::string(m_row[n]);
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
unsigned char sql::mariadb::recordset::get_uchar(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return (unsigned char)m_row[n][0];
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
unsigned int sql::mariadb::recordset::get_uint(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return (unsigned int)std::stoul(m_row[n]);
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
unsigned long sql::mariadb::recordset::get_ulong(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return std::stoul(m_row[n]);
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
unsigned long long sql::mariadb::recordset::get_ulonglong(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return std::stoull(m_row[n]);
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
unsigned short sql::mariadb::recordset::get_ushort(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return (unsigned short)std::atoi(m_row[n]);
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
wchar_t sql::mariadb::recordset::get_wchar(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return *((wchar_t *)m_row[n]);
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
std::wstring sql::mariadb::recordset::get_wstring(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return std::wstring((wchar_t *)m_row[n], mysql_fetch_lengths(m_ptr_res)[n] / 2);
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
const char * sql::mariadb::recordset::get_raw(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return m_row[n];
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
bool sql::mariadb::recordset::get_bool(unsigned long n) const
{
	assert(m_ptr_mysql != nullptr);
	assert(m_ptr_res != nullptr);
	assert(m_row != nullptr);
	assert(n < mysql_num_fields(m_ptr_res));
	return (*m_row[n]) - '0' != 0;
}


//*********************************************************
// 函数名称 : recordset
// 作    者 : Gooeen
// 完成日期 : 2015/09/15
// 函数说明 : 由 command 类对象创建 recordset 类对象
// 访问方式 : private
// 函数参数 : MYSQL * pointer 数据库句柄
//*********************************************************
sql::mariadb::recordset::recordset(MYSQL * pointer) noexcept
	: m_ptr_mysql(pointer)
	, m_ptr_res(mysql_store_result(pointer))
	, m_row(nullptr)
{
}


//*********************************************************
// 函数名称 : command
// 作    者 : Gooeen
// 完成日期 : 2015/09/15
// 函数说明 : 转移构造函数, 转移后原对象不应该使用
// 访问方式 : public
// 函数参数 : command && executor 需要转移的对象
//*********************************************************
sql::mariadb::command::command(command && executor) noexcept
	: m_ptr_mysql(executor.m_ptr_mysql)
	, m_text(std::move(executor.m_text))
	, m_strings(std::move(executor.m_strings))
	, m_datas(std::move(executor.m_datas))
	, m_udatas(std::move(executor.m_udatas))
	, m_parameters(std::move(executor.m_parameters))
{
	executor.m_ptr_mysql = nullptr;
}


//*********************************************************
// 函数名称 : command
// 作    者 : Gooeen
// 完成日期 : 2015/09/15
// 函数说明 : 构造函数
// 访问方式 : public
// 函数参数 : const connection & connector 数据库连接类
//*********************************************************
sql::mariadb::command::command(const connection & connector) noexcept
	: m_ptr_mysql(connector.m_ptr_mysql)
{
}


//*********************************************************
// 函数名称 : command
// 作    者 : Gooeen
// 完成日期 : 2015/09/15
// 函数说明 : 构造函数
// 访问方式 : public
// 函数参数 : const connection & connector 数据库连接类
// 函数参数 : const std::string & text SQL语句
// 异    常 : 如果尝试分配空间失败则抛出 std::bad_alloc 异常
//*********************************************************
sql::mariadb::command::command(const connection & connector, const std::string & text)
	: command(connector, std::string(text))
{
}


//*********************************************************
// 函数名称 : command
// 作    者 : Gooeen
// 完成日期 : 2015/09/15
// 函数说明 : 构造函数
// 访问方式 : public
// 函数参数 : const connection & connector 数据库连接类
// 函数参数 : std::string && text SQL语句
//*********************************************************
sql::mariadb::command::command(const connection & connector, std::string && text) noexcept
	: m_ptr_mysql(connector.m_ptr_mysql)
	, m_text(std::move(text))
{
}


//*********************************************************
// 函数名称 : error
// 作    者 : Gooeen
// 完成日期 : 2015/09/15
// 函数说明 : 获取数据库操作失败的错误信息
// 访问方式 : public 
// 函数参数 : void
// 返 回 值 : std::string 错误信息
//*********************************************************
std::string sql::mariadb::command::error(void) const noexcept
try
{
	return std::string(mysql_error(m_ptr_mysql));
}
catch (const std::exception &)
{
	return std::string();
}


//*********************************************************
// 函数名称 : errorno
// 作    者 : Gooeen
// 完成日期 : 2015/09/15
// 函数说明 : 获取数据库操作失败的错误代号
// 访问方式 : public
// 返 回 值 : unsigned int 错误代号
//*********************************************************
unsigned int sql::mariadb::command::errorno(void) const noexcept
{
	return mysql_errno(m_ptr_mysql);
}


//*********************************************************
// 函数名称 : prepare
// 作    者 : Gooeen
// 完成日期 : 2015/09/15
// 函数说明 : 预存SQL语句; 可使用以下例子防注入
//            executer.prepare("select * from table where id = ?");
//            executer.add(12);
// 访问方式 : public
// 函数参数 : std::string && text
// 返 回 值 : void
//*********************************************************
void sql::mariadb::command::prepare(std::string && text) noexcept
{
	m_text = std::move(text);
}


//*********************************************************
// 函数名称 : prepare
// 作    者 : Gooeen
// 完成日期 : 2015/09/15
// 函数说明 : 预存SQL语句; 可使用以下例子防注入
// 访问方式 : public
// 函数参数 : const std::string & text SQL语句
// 返 回 值 : void
// 异    常 : 如果尝试分配空间失败则抛出 std::bad_alloc 异常
//*********************************************************
void sql::mariadb::command::prepare(const std::string & text)
{
	m_text = text;
}


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
// 返 回 值 : void
// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
//*********************************************************
void sql::mariadb::command::add(unsigned int pos, const std::string & text)
{
	this->add(pos, std::string(text));
}


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
// 返 回 值 : void
// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
//*********************************************************
void sql::mariadb::command::add(unsigned int pos, std::string && text)
{
	// 保存字符串
	m_strings.push_back(std::move(text));

	// 保存数据的信息
	const auto p = m_strings.back().c_str();
	const auto size = m_strings.back().size();
	const auto pair = std::make_pair(p, size);
	m_parameters[pos] = std::make_pair(false, pair);
}


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
// 返 回 值 : void
// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
//*********************************************************
void sql::mariadb::command::add(unsigned int pos, const std::vector<char> &data)
{
	this->add(pos, std::vector<char>(data));
}


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
// 返 回 值 : void
// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
//*********************************************************
void sql::mariadb::command::add(unsigned int pos, std::vector<char> &&data)
{
	// 保存数据
	m_datas.push_back(std::move(data));

	// 保存数据的信息
	const auto p = m_datas.back().data();
	const auto size = m_datas.back().size();
	const auto pair = std::make_pair(p, size);
	m_parameters[pos] = std::make_pair(false, pair);
}


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
// 返 回 值 : void
// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
//*********************************************************
void sql::mariadb::command::add(unsigned int pos, const std::vector<unsigned char> &data)
{
	this->add(pos, std::vector<unsigned char>(data));
}


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
// 返 回 值 : void
// 异    常 : 如果分配资源失败则抛出 std::bad_alloc 异常
//*********************************************************
void sql::mariadb::command::add(unsigned int pos, std::vector<unsigned char> &&data)
{
	// 保存数据
	m_udatas.push_back(std::move(data));

	// 保存数据的信息
	const auto p = (char *)m_udatas.back().data();
	const auto size = m_udatas.back().size();
	const auto pair = std::make_pair(p, size);
	m_parameters[pos] = std::make_pair(false, pair);
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
unsigned long sql::mariadb::command::escape(char * to, const char * from, unsigned long size) const noexcept
{
	assert(m_ptr_mysql != nullptr);
	assert(to != nullptr);
	assert(from != nullptr);
	return mysql_real_escape_string(m_ptr_mysql, to, from, size);
}


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
std::string sql::mariadb::command::escape(const char * data, unsigned long size) const
{
	auto buffer = this->escape_buffer(data, size);
	return std::string(buffer.begin(), buffer.end());
}


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
std::string sql::mariadb::command::escape_with_quote(const char * data, unsigned long size) const
{
	auto buffer = this->escape_buffer_with_quote(data, size);
	return std::string(buffer.begin(), buffer.end());
}


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
std::vector<char> sql::mariadb::command::escape_buffer(const char * data, unsigned long size) const
{
	assert(m_ptr_mysql != nullptr);
	assert(data != nullptr);

	std::vector<char> buffer(size * 2 + 1);
	auto n = mysql_real_escape_string(m_ptr_mysql, buffer.data(), data, size);
	buffer.resize(n);

	return buffer;
}


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
std::vector<char> sql::mariadb::command::escape_buffer_with_quote(const char * data, unsigned long size) const
{
	assert(m_ptr_mysql != nullptr);
	assert(data != nullptr);

	std::vector<char> buffer(size * 2 + 1 + 2);
	auto n = mysql_real_escape_string(m_ptr_mysql, buffer.data() + 1, data, size);
	buffer.resize(n + 2);
	buffer.front() = '\'';
	buffer.back() = '\'';
	return buffer;
}


//*********************************************************
// 函数名称 : execute
// 作    者 : Gooeen
// 完成日期 : 2015/09/16
// 函数说明 : 执行SQL语句
// 访问方式 : public
// 函数参数 : const char * text SQL语句
// 返 回 值 : bool 如果SQL语句执行成功返回true; 反之返回false
//*********************************************************
bool sql::mariadb::command::execute(const char * text) const noexcept
{
	assert(m_ptr_mysql != nullptr);
	return mysql_query(m_ptr_mysql, text) == 0;
}


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
bool sql::mariadb::command::execute(const char * text, unsigned long length) const noexcept
{
	assert(m_ptr_mysql != nullptr);
	return mysql_real_query(m_ptr_mysql, text, length) == 0;
}


//*********************************************************
// 函数名称 : execute
// 作    者 : Gooeen
// 完成日期 : 2015/09/16
// 函数说明 : 执行SQL语句
// 访问方式 : public
// 函数参数 : const std::string & text SQL语句
// 返 回 值 : bool 如果SQL语句执行成功返回true; 反之返回false
//*********************************************************
bool sql::mariadb::command::execute(const std::string & text) const noexcept
{
	assert(m_ptr_mysql != nullptr);
	return mysql_real_query(m_ptr_mysql, text.c_str(), text.size()) == 0;
}


//*********************************************************
// 函数名称 : execute
// 作    者 : Gooeen
// 完成日期 : 2015/09/16
// 函数说明 : 执行SQL语句
// 访问方式 : public
// 函数参数 : const std::vector<char> & data SQL语句
// 返 回 值 : bool 如果SQL语句执行成功返回true; 反之返回false
//*********************************************************
bool sql::mariadb::command::execute(const std::vector<char>& data) const noexcept
{
	assert(m_ptr_mysql != nullptr);
	return mysql_real_query(m_ptr_mysql, data.data(), data.size()) == 0;
}


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
bool sql::mariadb::command::execute(void) const
{
	assert(m_ptr_mysql != nullptr);
	assert(!m_text.empty());

	unsigned long size = 0; // 保存数据的总字节数, 用于开辟缓冲区

	// 遍历数据, 求出数据的总字节数
	for (const auto &parameter : m_parameters)
	{
		const auto isnum = parameter.second.first; // 数据是否一个数
		const auto count = parameter.second.second.second; // 数据大小

		// 如果不是一个数, 则额外加上两个单引号的长度
		size += isnum ? count : count + 2;
	}

	// 用于保存生成的SQL语句
	std::vector<char> statement;
	statement.reserve(m_text.size() + (size * 2 + m_parameters.size()));

	// 保存原SQL语句中遍历到 '?' 的个数
	unsigned int mark_count = 0;

	// 生成SQL语句
	for (auto ch : m_text)
	{
		// 如果字符不是问号, 则直接添加该字符到新语句的结尾
		if (ch != '?')
		{
			statement.push_back(ch);
		}
		else // 如果字符是问号
		{
			const auto &parameter = m_parameters.at(mark_count); // 数据
			const auto isnum = parameter.first; // 数据是否一个数
			const auto start = parameter.second.first; // 数据开始位置
			const auto length = parameter.second.second; // 数据大小

			// 如果参数是一个数, 则直接复制数据
			if (isnum)
			{
				const auto count = statement.size(); // 保存此时容器大小
				statement.resize(count + length); // 扩充容量
				const auto pointer = statement.data() + count; // 复制的开始位置
				memcpy(pointer, start, length); // 复制数据
			}
			else // 如果参数是字符串或者数据, 则转换数据并添加单引号
			{
				statement.push_back('\''); // 添加单引号

				const auto count = statement.size(); // 保存此时容器大小
				statement.resize(count + (length * 2 + 1)); // 扩充容量
				const auto pointer = statement.data() + count; // 复制的开始位置
				statement.resize(count + this->escape(pointer, start, length));

				statement.push_back('\''); // 添加单引号
			}

			++mark_count; // 增加一个问号的个数
		}
	}

	// 执行SQL语句并返回是否执行成功
	return mysql_real_query(m_ptr_mysql, statement.data(), statement.size()) == 0;
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
sql::mariadb::recordset sql::mariadb::command::execute_reader(const char * text) const
{
	if_false_throw(this->execute(text), text);
	return recordset(m_ptr_mysql);
}


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
sql::mariadb::recordset sql::mariadb::command::execute_reader(const char * text, unsigned long length) const
{
	if_false_throw(this->execute(text, length), std::string(text, length));
	return recordset(m_ptr_mysql);
}


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
sql::mariadb::recordset sql::mariadb::command::execute_reader(const std::string & text) const
{
	if_false_throw(this->execute(text), text);
	return recordset(m_ptr_mysql);
}


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
sql::mariadb::recordset sql::mariadb::command::execute_reader(const std::vector<char>& data) const
{
	if_false_throw(this->execute(data), std::string(data.begin(), data.end()));
	return recordset(m_ptr_mysql);
}


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
sql::mariadb::recordset sql::mariadb::command::execute_reader(void) const
{
	if_false_throw(this->execute(), m_text);
	return recordset(m_ptr_mysql);
}
