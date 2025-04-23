#pragma once

#include <vector>
#include <string>
#include <chrono>

namespace NotificationManager
{
	struct Notification
	{
		std::string title;
		std::string message;
		std::chrono::time_point<std::chrono::system_clock> startTime;

		Notification(const char* t, const char* m, std::chrono::time_point<std::chrono::system_clock> s)
			: title(t), message(m), startTime(s) {}
	};

	bool Render();
	bool RenderNotification(Notification notification, int x, int y, int width, int height);

	bool Send(const char* title, const char* format, ...);

	static std::vector<Notification> notifications;

	static const int max_notifications = 3;
	static const float font_size = 28;

	static const double ALIVE_TIME_S = 5;
	static const double SLIDE_OUT_TIME_S = 0.5;
	static const double SLIDE_IN_TIME_S = 0.5;
};

#define NotificationInfo(message, ...) NotificationManager::Send("Sincara+ :: Info", message, ##__VA_ARGS__)
#define NotificationWarning(message, ...) NotificationManager::Send("Sincara+ :: Warning", message, ##__VA_ARGS__)
#define NotificationError(message, ...) NotificationManager::Send("Sincara+ :: Error", message, ##__VA_ARGS__)