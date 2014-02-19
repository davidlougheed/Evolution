void console_log(std::string message) {
	printf("%ld: %s\n", time(NULL), message.c_str());
}