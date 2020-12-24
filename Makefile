include CONFIG.cfg

.PHONY: all, check, clean

SOURCES = $(wildcard $(SOURCE_DIR)/*.c)

RESULTS_DIR = results
TEST_FILENAMES = $(wildcard $(TEST_DIR)/*.in)
TMP_STDOUT_FILENAME = stdout.buf

SUCCESS_MESSAGE = GOOD

TARGET_DEPENDENCIES = $(subst $(SOURCE_DIR), $(BUILD_DIR), $(SOURCES:.c=.o))
RESULTS = $(subst $(TEST_DIR), $(RESULTS_DIR), $(TEST_FILENAMES:.in=.txt))

all: $(BUILD_DIR) $(BUILD_DIR)/$(NAME)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) -c $< -o $@

$(BUILD_DIR)/$(NAME): $(TARGET_DEPENDENCIES)
	$(CC) $^ -o $@

$(BUILD_DIR):
	mkdir $@

$(RESULTS_DIR):
	mkdir $@

$(RESULTS_DIR)/%.txt: $(TEST_DIR)/%.in $(TEST_DIR)/%.out $(BUILD_DIR)/$(NAME)
	program_out_filename=$(RESULTS_DIR)/$(TMP_STDOUT_FILENAME); \
	@$(BUILD_DIR)/$(NAME) $< > $$program_out_filename \
	test_out_filename=$$( echo $< | sed 's/.in/.out/g'); \
	cmp $$test_out_filename $$program_out_filename > $@; \
	if [ $$? = 0 ]; \
    	then echo $(SUCCESS_MESSAGE) > $@; \
    fi

check: $(RESULTS_DIR) $(RESULTS) all
	@is_test_failed=0; \
	for filename in $(RESULTS); \
	do \
		echo "Test $$filename:"; \
		cat $$filename; \
		if [ "$$(cat $$filename)" != "$(SUCCESS_MESSAGE)" ]; \
		then is_test_failed=$$(($$is_test_failed + 1)); \
		fi; \
	done; \
	if [ $$is_test_failed != 0 ]; \
	then exit 1; \
	fi; \

clean:
	rm -rf $(BUILD_DIR)/*
	rm -rf $(RESULTS_DIR)