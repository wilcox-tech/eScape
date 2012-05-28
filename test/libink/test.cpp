#include <libink/WTDictionary.h>
#include <libink/WTFormParser.h>
#include "../test.h"

bool insert_into_managed(WTDictionary *dict)
{
	dict->set("Key 1", strdup("Value 1"));
	dict->set("Key 2", strdup("Value 2"));
	dict->set("To Delete 1", strdup("Hi!"));
	dict->set("Key 3", strdup("Value 3"));
	dict->set("Key 4", strdup("Value 4"));
	dict->set("Key 4a", strdup("Old value"));
	dict->set("To Delete 2", strdup("Hi!"));
	dict->set("Key 5", strdup("Value 5"));
	dict->set("To Delete 3", strdup("Hi!"));
	dict->set("Key 6", strdup("Value 6"));
	dict->set("Key 7", strdup("Value 7"));
	dict->set("Key 7a", strdup("Old value"));
	dict->set("Key 8", strdup("Value 8"));
	dict->set("To Delete 4", strdup("Hi!"));
	dict->set("Key 9", strdup("Value 9"));
	dict->set("Key 9a", strdup("Old value"));
	dict->set("Key 9b", strdup("Old value"));
	return true;
};

bool remove_managed(WTDictionary *dict)
{
	dict->set("To Delete 1", NULL);
	dict->set("To Delete 2", NULL);
	dict->set("To Delete 3", NULL);
	dict->set("To Delete 4", NULL);
	return true;
};

bool replace_managed(WTDictionary *dict)
{
	dict->set("Key 4a", strdup("New value"));
	dict->set("Key 7a", strdup("New value"));
	dict->set("Key 9a", strdup("New value"));
	return true;
};

bool nonmanaged_in_managed(WTDictionary *dict)
{
	char *val1, *val2, *val3;
	val1 = strdup("This will be around after release!");
	val2 = strdup("As will this value");
	val3 = strdup("This one, too");
	dict->set("Nonmanaged 1", val1, WTDICT_KEY_UNMANAGED);
	dict->set("Nonmanaged 2", val2, WTDICT_KEY_UNMANAGED);
	dict->set("Nonmanaged 3", val3, WTDICT_KEY_UNMANAGED);
	
	dict->set("Nonmanaged 1", NULL);
	dict->set("Nonmanaged 2", NULL);
	
	printf("we still have %s, %s, %s\n", val1, val2, val3);
	free(val1); free(val2); free(val3);
	
	return true;
};

bool managed_in_nonmanaged(WTDictionary *dict)
{
	char *val1, *val2, *val3;
	val1 = strdup("This will be removed after release!");
	val2 = strdup("This value will be freed after set to NULL");
	val3 = strdup("The quick brown fox jumps over the lazy dog.");
	
	dict->set("Managed 1", val1, WTDICT_KEY_MANAGED);
	dict->set("Managed 2", val2, WTDICT_KEY_MANAGED);
	dict->set("Managed 3", val3, WTDICT_KEY_MANAGED);
	
	dict->set("Managed 1", "This will NOT be released", WTDICT_KEY_UNMANAGED);
	dict->set("Managed 2", NULL);
	
	return true;
}

void test_dict(void)
{
	WTDictionary *dict1, *dict2;
	WTSizedBuffer *buff;

	DO_TEST(
		"Create dictionary #1 (managed)",
		(dict1 = new WTDictionary(true)),
		NOTHING,
		NOTHING
		)

	DO_TEST(
		"Create dictionary #2 (non-managed)",
		(dict2 = new WTDictionary(false)),
		NOTHING,
		NOTHING
		)

	DO_TEST(
		"Insert values into managed dictionary",
		insert_into_managed(dict1),
		NOTHING,
		NOTHING
		)

	printf("Values:");
	buff = dict1->all();
	printf("%s\n",buff->buffer);


	DO_TEST(
		"Remove a few values from managed dictionary",
		remove_managed(dict1),
		NOTHING,
		NOTHING
		)

	printf("Values:");
	buff = dict1->all();
	printf("%s\n",buff->buffer);

	DO_TEST(
		"Replace a few values in managed dictionary",
		replace_managed(dict1),
		NOTHING,
		NOTHING
		)

	printf("Values:");
	buff = dict1->all();
	printf("%s\n",buff->buffer);
	
	DO_TEST(
		"Add some non-managed values to managed dictionary and remove them",
		nonmanaged_in_managed(dict1),
		NOTHING,
		NOTHING
		)
	
	DO_TEST(
		"Add some managed values to non-managed dictionary and remove them",
		managed_in_nonmanaged(dict2),
		NOTHING,
		NOTHING
		)

	DO_TEST(
		"Deallocate managed dictionary",
		true,
		NOTHING,
		NOTHING
		)
	delete dict1;

	DO_TEST(
		"Deallocate non-managed dictionary",
		true,
		NOTHING,
		NOTHING
		)
	delete dict2;
};


void test_form_parser(void)
{
};


int main(void)
{
	print_header("libink");
	
	mowgli_init();
	
	test_dict();
	test_form_parser();
	
	PRINT_STATS
	
	return 0;
};

