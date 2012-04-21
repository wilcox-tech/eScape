#include <libGwen/WTMIMEEncoder.h>
#include "../test.h"

bool single(WTMIMEAttachment *attach)
{
	char *result = WTMIMEEncoder::encode_single(attach);
	if(result == NULL) return false;
	free(result);
	return true;
}

bool multiple(vector <WTMIMEAttachment *> multi)
{
	char *result = WTMIMEEncoder::encode_multiple(multi);
	if(result == NULL) return false;
	free(result);
	return true;
}

int main(void)
{
	WTMIMEAttachment *attach, *attach2;
	vector<WTMIMEAttachment *> multi;
	
	print_header("libGwen MIME functionality");
	
	DO_TEST("MIME message with no attachments",
		(WTMIMEEncoder::encode_multiple(multi) == NULL),
		NOTHING,
		NOTHING)
	
	attach = new WTMIMEAttachment;
	attach->datatype = MIME_DATATYPE_BUFFER;
	attach->data.buffer = "This text will be encoded in base64.";
	attach->length = strlen((const char *)attach->data.buffer);
	attach->type = "text/plain";
	
	DO_TEST("MIME message with one attachment"
		single(attach),
		NOTHING,
		NOTHING)
	
	attach2 = new WTMIMEAttachment;
	attach2->datatype = MIME_DATATYPE_BUFFER;
	attach2->type = "image/png";
	
	{
		char *png = NULL;
		FILE *f;
		bool keep_going = true;
		size_t chunk_size = 128, completed = 0, bytes_read = 0;
		
		if((f = fopen("test.png", "r")) == NULL)
		{
			fprintf(stderr, "Couldn't open test.png: %s\n", strerror(errno));
			return -1;
		}
		
		while(keep_going) {
		png = static_cast<char *>(realloc(png, completed+chunk_size));
                if(png == NULL)
                        alloc_error("data buffer", completed+chunk_size);

                bytes_read = fread(png+completed, sizeof(char), chunk_size, f);
                if(bytes_read < chunk_size)
                {
                        if(ferror(f)) fatal_error("couldn't read")
                        else if(feof(f)) keep_going = false;
                }

                completed += bytes_read;
		}
		
		attach2->length = completed;
		attach2->data.buffer = png;
		
		fclose(f);
	}
	
	multi.push_back(attach);
	multi.push_back(attach2);
	
	DO_TEST("MIME message with multiple attachments",
		multiple(multi),
		NOTHING,
		NOTHING)
	
	attach->transfer_enc = MIME_TRANSFER_BINARY;
	
	DO_TEST("MIME message with base64 + binary attachments",
		multiple(multi),
		NOTHING,
		NOTHING)
	
	attach->transfer_enc = MIME_TRANSFER_BASE64;
	attach2->transfer_enc= MIME_TRANSFER_BINARY;
	
	DO_TEST("MIME message with binary data attachment",
		multiple(multi),
		NOTHING,
		NOTHING)
	
	free((void *)attach2->data.buffer);
	delete attach2;
	delete attach;
	
	PRINT_STATS
	
	return 0;
}

