#pragma once

static yo_string_t yo_load_file(char *file_name)
{
    yo_string_t ret = { 0 };

    FILE *file = fopen(file_name, "rb");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t *data = yo_heap_alloc(size + 1, false);

    if (data)
    {
        size = fread(data, 1, size, file);

        ret.data   = data;
        ret.length = size;

        data[size] = '\0';
    }

    fclose(file);

    return ret;
}

static void yo_unload_file(yo_string_t file_content)
{
    yo_heap_free(file_content.data);
}
