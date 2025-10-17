#include <gtest/gtest.h>
#include <pgrender/eventSystem.h>

TEST(WindowEventQueueTest, PushAndPop) {
	pgrender::WindowEventQueue queue;

	EXPECT_EQ(queue.size(), 0u);
	EXPECT_TRUE(queue.empty());

	pgrender::Event event1{};
	event1.type = pgrender::EventType::KeyPress;
	event1.timestamp = 1000;

	queue.pushEvent(event1);

	EXPECT_EQ(queue.size(), 1u);
	EXPECT_FALSE(queue.empty());

	auto popped = queue.tryPopEvent();
	ASSERT_TRUE(popped.has_value());
	EXPECT_EQ(popped->type, pgrender::EventType::KeyPress);
	EXPECT_EQ(popped->timestamp, 1000u);

	EXPECT_TRUE(queue.empty());
}

TEST(WindowEventQueueTest, MultipleEvents) {
	pgrender::WindowEventQueue queue;

	// Push 3 eventos
	for (int i = 0; i < 3; ++i) {
		pgrender::Event event{};
		event.type = pgrender::EventType::MouseMove;
		event.timestamp = i * 100;
		queue.pushEvent(event);
	}

	EXPECT_EQ(queue.size(), 3u);

	// Pop todos
	int count = 0;
	while (auto event = queue.tryPopEvent()) {
		EXPECT_EQ(event->type, pgrender::EventType::MouseMove);
		EXPECT_EQ(event->timestamp, count * 100);
		count++;
	}

	EXPECT_EQ(count, 3);
	EXPECT_TRUE(queue.empty());
}

TEST(WindowEventQueueTest, EventFilter) {
	pgrender::WindowEventQueue queue;

	// Filtro que solo acepta eventos de teclado
	queue.setEventFilter([](const pgrender::Event& event) {
		return event.type == pgrender::EventType::KeyPress ||
			event.type == pgrender::EventType::KeyRelease;
		});

	// Intentar añadir evento de teclado (debería pasar)
	pgrender::Event keyEvent{};
	keyEvent.type = pgrender::EventType::KeyPress;
	queue.pushEvent(keyEvent);

	EXPECT_EQ(queue.size(), 1u);

	// Intentar añadir evento de ratón (debería ser filtrado)
	pgrender::Event mouseEvent{};
	mouseEvent.type = pgrender::EventType::MouseMove;
	queue.pushEvent(mouseEvent);

	EXPECT_EQ(queue.size(), 1u); // No cambió

	// Verificar que solo está el evento de teclado
	auto popped = queue.tryPopEvent();
	ASSERT_TRUE(popped.has_value());
	EXPECT_EQ(popped->type, pgrender::EventType::KeyPress);
}

TEST(WindowEventQueueTest, EventWatcher) {
	pgrender::WindowEventQueue queue;

	int watchCount = 0;

	queue.setEventWatcher([&watchCount](const pgrender::Event&) {
		watchCount++;
		return true;
		});

	pgrender::Event event1{};
	event1.type = pgrender::EventType::KeyPress;
	queue.pushEvent(event1);

	EXPECT_EQ(watchCount, 1);

	pgrender::Event event2{};
	event2.type = pgrender::EventType::MouseMove;
	queue.pushEvent(event2);

	EXPECT_EQ(watchCount, 2);
	EXPECT_EQ(queue.size(), 2u);
}

TEST(WindowEventQueueTest, Clear) {
	pgrender::WindowEventQueue queue;

	for (int i = 0; i < 10; ++i) {
		pgrender::Event event{};
		queue.pushEvent(event);
	}

	EXPECT_EQ(queue.size(), 10u);

	queue.clear();

	EXPECT_TRUE(queue.empty());
	EXPECT_EQ(queue.size(), 0u);
}
