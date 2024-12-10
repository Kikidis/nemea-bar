all:
	+$(MAKE) -C creator
	+$(MAKE) -C monitor
	+$(MAKE) -C visitor
	+$(MAKE) -C receptionist

clean:
	+$(MAKE) -C creator clean
	+$(MAKE) -C monitor clean
	+$(MAKE) -C visitor clean
	+$(MAKE) -C receptionist clean