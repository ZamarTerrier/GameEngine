
#include "e_widget_entry_area.h"

#include <GLFW/glfw3.h>

#include "engine.h"

#include "e_resource.h"

extern void EntryWidgetPress(EWidget *widget, void *entry, void *arg);
extern void EntryWidgetUnfocus(EWidget *widget, void *entry, void *arg);
extern void EntryWidgetCharInput(EWidget* widget, uint32_t codepoint, void *arg);

bool e_var_backspace = false;

EWidgetText *EntryAreaWidgetMakeAnotherText(EWidgetEntryArea * area)
{
    DrawParam dParam;
    memset(&dParam, 0, sizeof(DrawParam));

    area->entry.num_texts ++;
    area->entry.curr_texts++;

    EWidgetText *text = calloc(1, sizeof(EWidgetText));
    TextWidgetInit(text, area->entry.text.tData.font.fontSize, dParam, &area->entry.widget);
    Transform2DSetPosition(text, 0, area->entry.text.tData.font.fontSize * 2 * area->entry.num_texts);

}
void EntryAreaWidgetMakeAnotherLine(EWidgetEntryArea * area)
{
    area->entry.num_lines ++;
    area->entry.curr_line ++;

    area->entry.buffers = realloc(area->entry.buffers, area->entry.num_lines * sizeof(uint32_t *));
    area->entry.buffers[area->entry.curr_line] = calloc(BUFFER_SIZE, sizeof(uint32_t));
    memset(area->entry.buffers[area->entry.curr_line], 0, BUFFER_SIZE * sizeof(uint32_t));

    area->entry.currPos = 0;
}

void EntryAreaWidgetMakeDelete(EWidgetEntryArea *area)
{

    area->entry.buffers[area->entry.curr_line][area->entry.currPos] = 0;

    area->entry.currPos--;

    if(area->entry.currPos < 0)
    {
        area->entry.currPos = 0;

        if(area->entry.num_texts > 1)
        {
            area->textHeight -= area->entry.text.tData.font.fontSize / 7 * 20;

            if(area->textHeight  <= area->entry.height){
                ChildStack *child = WidgetFindChild(&area->entry.widget, area->entry.curr_texts);

                if(child->next != NULL && child->before != NULL)
                {
                    ChildStack *next = child->next;
                    ChildStack *before = child->before;

                    GameObject *go = child->node;
                    go->ToBeFree = true;
                    child->node = NULL;

                    free(child);
                    next->before = before;
                    before->next = next;

                }else if(child->next != NULL){

                    GameObject *go = child->node;
                    go->ToBeFree = true;
                    child->node = NULL;

                    child->next->before = NULL;
                    area->entry.widget.child = child->next;
                    free(child);

                }else{

                    GameObject *go = child->node;
                    go->ToBeFree = true;
                    child->node = NULL;

                    if(child->before != NULL)
                    {
                        child->before->next = NULL;
                        area->entry.widget.last = child->before;
                    }

                    free(child);
                    child = NULL;
                }

                area->entry.curr_texts --;
                area->entry.num_texts --;
            }

            if(area->textHeight > area->entry.height)
            {
                for(int j = 0; j < area->entry.num_texts; j++)
                {
                    EWidgetText *text = WidgetFindChild(&area->entry.widget, area->entry.num_texts - 1 - j)->node;

                    TextWidgetSetText(text, area->entry.buffers[area->entry.num_lines - 1 - j]);
                }
            }

            free(area->entry.buffers[area->entry.curr_line]);

            area->entry.curr_line --;
            area->entry.num_lines --;

            area->entry.buffers = realloc(area->entry.buffers, area->entry.num_lines * sizeof( uint32_t *));

            area->entry.currPos = ToolsStr32BitLength(area->entry.buffers[area->entry.curr_line]);
        }
    }else {


        if(area->entry.buffers[area->entry.curr_line][area->entry.currPos] < 0)
        {
            area->entry.buffers[area->entry.curr_line][area->entry.currPos] = 0;

            area->entry.currPos--;
        }

        area->entry.buffers[area->entry.curr_line][area->entry.currPos] = 0;

        EWidgetText *text = WidgetFindChild(&area->entry.widget, area->entry.curr_texts)->node;
        TextWidgetSetText(text, area->entry.buffers[area->entry.curr_line]);
    }
}

void EntryAreaWidgetInsertText(EWidgetEntryArea *area, const char *src)
{    
    uint32_t size = strlen(src);

    char *point = &area->entry.buffers[area->entry.curr_line][area->entry.currPos];


    int iter = 0;
    for(int i=0;i < size;i++)
    {
        point[iter] = src[i];

        area->entry.currPos ++;
        iter ++;

        EWidgetText *text = WidgetFindChild(&area->entry.widget, area->entry.curr_line)->node;
        TextWidgetSetText(text, area->entry.buffers[area->entry.curr_line]);

        if((text->tData.textWidth >= area->entry.width || point[iter - 1] == '\n') && area->textHeight < area->entry.height)
        {
            area->textHeight += text->tData.font.fontSize / 7 * 20;

            EntryAreaWidgetMakeAnotherText(area);
            EntryAreaWidgetMakeAnotherLine(area);

            point = &area->entry.buffers[area->entry.curr_line][area->entry.currPos];

            iter = 0;
        }
        else if((text->tData.textWidth >= area->entry.width || point[iter - 1] == '\n') && area->textHeight >= area->entry.height)
        {
            area->textHeight += text->tData.font.fontSize / 7 * 20;

            EntryAreaWidgetMakeAnotherLine(area);

            for(int j = 0; j < area->entry.num_texts; j++)
            {
                text = WidgetFindChild(&area->entry.widget, area->entry.num_texts - 1 - j)->node;
                TextWidgetSetText(text, area->entry.buffers[area->entry.num_lines - 1 - j]);
            }

            point = &area->entry.buffers[area->entry.curr_line][area->entry.currPos];

            iter = 0;
        }
    }
}
void EntryAreaWidgetKeyPressInput(EWidget* widget, int key, void *arg){

    EWidgetEntryArea *temp = widget;

    if(key == GLFW_KEY_BACKSPACE)
        EntryAreaWidgetMakeDelete(temp);

    if(key == GLFW_KEY_ENTER)
    {

        temp->entry.buffers[temp->entry.curr_line][temp->entry.currPos] = '\n';

        EWidgetText *text = WidgetFindChild(&temp->entry.widget, temp->entry.curr_line)->node;

        if(temp->textHeight < temp->entry.height)
        {
            temp->textHeight += text->tData.font.fontSize / 7 * 20;

            TextWidgetSetText(text, temp->entry.buffers[temp->entry.curr_line]);

            EntryAreaWidgetMakeAnotherText(temp);
            EntryAreaWidgetMakeAnotherLine(temp);

        }else if(temp->textHeight >= temp->entry.height)
        {
            temp->textHeight += text->tData.font.fontSize / 7 * 20;

            EntryAreaWidgetMakeAnotherLine(temp);

            for(int j = 0; j < temp->entry.num_texts; j++)
            {
                text = WidgetFindChild(&temp->entry.widget, temp->entry.num_texts - 1 - j)->node;
                TextWidgetSetText(text, temp->entry.buffers[temp->entry.num_lines - 1 - j]);
            }

        }

        temp->entry.currPos = 0;

        return;
    }

    if(e_ctrl_press == true && e_v_press == true && !e_pasted)
    {

        char *text = glfwGetClipboardString(e_window);

        EntryAreaWidgetInsertText(temp, text);

        e_pasted = true;
    }

    EWidgetText *text = WidgetFindChild(&temp->entry.widget, temp->entry.curr_texts)->node;
    TextWidgetSetText(text, temp->entry.buffers[temp->entry.curr_line]);
}

void EntryAreaWidgetKeyRepeatInput(EWidget* widget, int key, void *arg)
{
    if(key == GLFW_KEY_BACKSPACE)
        EntryAreaWidgetMakeDelete(widget);
}

void EntryAreaWidgetInit(EWidgetEntryArea *entry, int fontSize, EWidget* parent){
    DrawParam dParam = {};

    if(fontSize > 16)
        fontSize = 16;
    else if(fontSize <= 0)
        fontSize = 2;

    WidgetInit(&entry->entry.widget, dParam, parent);
    entry->entry.widget.color = (vec4){0.7, 0.7, 0.7, 1.0f};

    TextWidgetInit(&entry->entry.text, fontSize, dParam, &entry->entry.widget);
    Transform2DSetPosition(&entry->entry.text, 0, fontSize * 2);
    entry->entry.num_texts = 1;
    entry->entry.curr_texts = 0;

    entry->entry.currPos = 0;

    entry->entry.buffers = calloc(1, sizeof(uint32_t *));
    entry->entry.buffers[0] = calloc(BUFFER_SIZE, sizeof(uint32_t));
    memset(entry->entry.buffers[0], 0, BUFFER_SIZE * sizeof(uint32_t));
    entry->entry.num_lines = 1;
    entry->entry.curr_line = 0;
    entry->entry.currPos = 0;

    WidgetConnect(entry, GUI_TRIGGER_MOUSE_PRESS, EntryWidgetPress, NULL);
    WidgetConnect(entry, GUI_TRIGGER_WIDGET_UNFOCUS, EntryWidgetUnfocus, NULL);

    WidgetConnect(entry, GUI_TRIGGER_ENTRY_CHAR_INPUT, EntryWidgetCharInput, NULL);

    WidgetConnect(entry, GUI_TRIGGER_ENTRY_KEY_PRESS_INPUT, EntryAreaWidgetKeyPressInput, NULL);
    WidgetConnect(entry, GUI_TRIGGER_ENTRY_KEY_REPEAT_INPUT, EntryAreaWidgetKeyRepeatInput, NULL);
}
