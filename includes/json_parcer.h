#ifndef JSON_PARCER_H
#define JSON_PARCER_H

typedef enum{
    gltf_type_none,
    gltf_type_string,
    gltf_type_scalar,
    gltf_type_float,
    gltf_type_struct,
    gltf_type_array,
    gltf_type_head,
} engine_gltf_type_num;

typedef struct{
    char *name;
    void **data;
    size_t size;
    engine_gltf_type_num type;
} json_struct;


int read_string(const char *point, char *out)
{
    int iter = 0;


    if(point[iter] == '"')
    {
        iter++;

        int len =0;
        char *start= &point[iter];

        while (point[iter] != '"')
        {
            len++;
            iter++;
        }

        iter++;
        char* char_point;
        out = calloc(len + 1, sizeof(char));
        char_point = out;

        for (int i=0; i < len; i++)
        {
            char_point[i] = start[i];
        }

        char_point[len] = '\0';
    }

    return iter;
}

int read_value(char *point, json_struct *out)
{

    if(point == NULL)
        return 0;

    out = calloc(1, sizeof(json_struct));

    int len = 0, iter= 0;

    char buff[256];

    while(point[iter] == ' ')
        iter++;

    if(point[iter] == '"')
        iter++;

    char *temp = &point[iter];

    while (point[iter] != ',' && point[iter] != ']') {
        if(point[iter] == '.' )
            out->type = gltf_type_float;
        else if(point[iter] == '"' )
            out->type = gltf_type_string;

        len ++;
        iter ++;
    }

    if(out->type == gltf_type_none)
        out->type = gltf_type_scalar;

    if(len > 0)
    {
        memcpy(buff, temp, len);

        for(int i=0;i<len;i++)
        {
            if(buff[i] == '.')
                buff[i] = ',';
        }

        char* char_point;
        int* int_point;
        double* float_point;

        switch(out->type)
        {
            case gltf_type_string:
                out->data = calloc(out->size, sizeof(char*));
                char_point = out->data;
                char_point[out->size - 1] = calloc(len, sizeof(char));
                memcpy(&out->data[out->size - 1], buff, len);
                break;
            case gltf_type_float:
                out->data = calloc(out->size, sizeof(double*));
                float_point = out->data;
                sscanf(buff, "%lf", float_point);
                break;
            case gltf_type_scalar:
                out->data = calloc(out->size, sizeof(int*));
                int_point = out->data;
                *int_point = atoi(buff);
                break;
            case gltf_type_none:
                break;
            default:
                break;
        }
    }

    return iter;
}

int read_array(const char *point, json_struct *out){

    int iter = 0;

    if(point[iter] == '[')
    {
        out = calloc(1, sizeof(json_struct));
        out->size = 0;
        out->type = gltf_type_array;

        iter++;
        while(point[iter] != ']')
        {
            if(point[iter] == '{')
            {
                out->data = calloc(1, sizeof(json_struct*));
                out->size ++;

                json_struct *json_point = out->data;

                iter += read_struct(&point[iter], &json_point[out->size - 1]);

            }else if(point[iter] != ' ' && point[iter] != '\n')
            {
                out->data = calloc(1, sizeof(json_struct*));

                json_struct *json_point = out->data;

                iter += read_value(&point[iter], &json_point[out->size - 1]);
            }

            if(point[iter] == ',')
            {
                out->size++;
            }

            if(point[iter] != ']')
                iter++;
        }
        iter++;

        return iter;
    }
}

int read_struct(const char *point, json_struct *out)
{

    int iter = 0;

    if(point[iter] == '{')
    {
        iter++;
        out = calloc(1, sizeof(json_struct));
        out->type = gltf_type_struct;

        while(point[iter] != '}')
        {
            if(point[iter] == '"')
            {
                iter += read_string(&point[iter], out->name);
            }

            if(point[iter] == ':')
            {
                iter++;

                while(point[iter] == ' ')
                    iter++;


                if(point[iter] == '[')
                {
                    out->size ++;
                    out->data = realloc(out->data, out->size * sizeof(json_struct*));

                    json_struct *json_point = out->data;

                    iter += read_array(&point[iter], &json_point[out->size - 1]);
                }else if(point[iter] == '{')
                {
                    out->size ++;
                    out->data = realloc(out->data, out->size * sizeof(json_struct*));

                    json_struct *json_point = out->data;

                    iter += read_struct(&point[iter], &json_point[out->size - 1]);
                }else
                {
                    out->size ++;
                    out->data = realloc(out->data, out->size * sizeof(json_struct*));
                    json_struct *json_point = out->data;

                    iter += read_value(&point[iter], &json_point[out->size - 1]);
                }
            }

            iter++;
        }

        iter++;
    }
    return iter;
}

void read_json(char *buffer, int size, json_struct *json)
{
    int iter = 0;
    while(iter <= size)
    {

        if(buffer[iter] == '{')
        {

            json->data = calloc(1, sizeof(json_struct*));
            json->size ++;

            json_struct *json_point = json->data;

            iter +=read_struct(&buffer[iter], &json_point[json->size - 1]);
        }

        iter ++;
    }
}

#endif // JSON_PARCER_H
