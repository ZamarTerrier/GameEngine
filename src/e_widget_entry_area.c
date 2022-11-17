
#include "e_widget_entry_area.h"

#include <GLFW/glfw3.h>

#include "e_resource.h"

extern void EntryWidgetPress(EWidget *widget, void *entry, void *arg);
extern void EntryWidgetUnfocus(EWidget *widget, void *entry, void *arg);
extern void EntryWidgetCharInput(EWidget* widget, uint32_t codepoint, void *arg);

EWidgetText *EntryAreaMakeAnotherLine(EWidgetEntryArea * area)
{
    DrawParam dParam;
    memset(&dParam, 0, sizeof(DrawParam));

    area->entry.num_lines ++;
    area->entry.curr_line++;

    EWidgetText *text = calloc(1, sizeof(EWidgetText));
    TextWidgetInit(text, area->entry.text.tData.font.fontSize, dParam, &area->entry.widget);
    Transform2DSetPosition(text, 0, area->entry.text.tData.font.fontSize * 2 * area->entry.num_lines);

    area->entry.buffers = realloc(area->entry.buffers, area->entry.num_lines * sizeof(uint32_t *));
    area->entry.buffers[area->entry.curr_line] = calloc(BUFFER_SIZE, sizeof(uint32_t));
    memset(area->entry.buffers[area->entry.curr_line], 0, BUFFER_SIZE * sizeof(uint32_t));

    area->entry.currPos = 0;

    return text;
}

void EntryAreaWidgetKeyPressInput(EWidget* widget, int key, void *arg){

    EWidgetEntryArea *temp = widget;

    if(key == GLFW_KEY_BACKSPACE)
    {
        temp->entry.buffers[temp->entry.curr_line][temp->entry.currPos] = 0;

        temp->entry.currPos--;

        if(temp->entry.currPos < 0)
        {
            temp->entry.currPos = 0;

            if(temp->entry.num_lines > 1)
            {
                ChildStack *child = WidgetFindChild(&temp->entry.widget, temp->entry.curr_line);

                if(child->next != NULL && child->before != NULL)
                {
                    ChildStack *next = child->next;
                    ChildStack *before = child->before;

                    WidgetDestroy(child->node);
                    free(child->node);
                    child->node = NULL;

                    free(child);
                    next->before = before;
                    before->next = next;

                }else if(child->next != NULL){


                    WidgetDestroy(child->node);
                    free(child->node);
                    child->node = NULL;

                    child->next->before = NULL;
                    temp->entry.widget.child = child->next;
                    free(child);

                }else{
                    WidgetDestroy(child->node);
                    free(child->node);
                    child->node = NULL;

                    if(child->before != NULL)
                    {
                        child->before->next = NULL;
                        temp->entry.widget.last = child->before;
                    }

                    free(child);
                    child = NULL;
                }

                free(temp->entry.buffers[temp->entry.curr_line]);

                temp->entry.curr_line --;
                temp->entry.num_lines --;

                temp->entry.buffers = realloc(temp->entry.buffers, temp->entry.num_lines * sizeof( uint32_t *));

                temp->entry.currPos = ToolsStr32BitLength(temp->entry.buffers[temp->entry.curr_line]);

            }
        }

        temp->entry.buffers[temp->entry.curr_line][temp->entry.currPos] = 0;
    }

    if(key == GLFW_KEY_ENTER)
    {

        temp->entry.buffers[temp->entry.curr_line][temp->entry.currPos] = 0;

        EWidgetText *text = WidgetFindChild(&temp->entry.widget, temp->entry.curr_line)->node;
        TextWidgetSetText(text, temp->entry.buffers[temp->entry.curr_line]);

        EntryAreaMakeAnotherLine(temp);

        temp->lineNum ++;

        return;
    }

    if(e_ctrl_press == true && e_v_press == true && !e_pasted)
    {
        uint32_t *point = &temp->entry.buffers[temp->entry.curr_line][temp->entry.currPos];

        char *text = glfwGetClipboardString(e_window);

        uint32_t size = strlen(text);

        int iter = 0;
        for(int i=0;i < size;i++)
        {
            point[iter] = text[i];

            temp->entry.currPos ++;
            iter ++;

            if(temp->entry.currPos + 1 >= temp->entry.width)
            {
                EWidgetText *text = WidgetFindChild(&temp->entry.widget, temp->entry.curr_line)->node;
                TextWidgetSetText(text, temp->entry.buffers[temp->entry.curr_line]);

                EntryAreaMakeAnotherLine(temp);

                temp->lineNum ++;

                point = &temp->entry.buffers[temp->entry.curr_line][temp->entry.currPos];

                iter = 0;
            }
        }

        e_pasted = true;
    }

    EWidgetText *text = WidgetFindChild(&temp->entry.widget, temp->entry.curr_line)->node;

    TextWidgetSetText(text, temp->entry.buffers[temp->entry.curr_line]);
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


    entry->entry.buffers = calloc(1, sizeof(uint32_t *));
    entry->entry.buffers[0] = calloc(BUFFER_SIZE, sizeof(uint32_t));
    memset(entry->entry.buffers[0], 0, BUFFER_SIZE * sizeof(uint32_t));
    entry->entry.num_lines = 1;
    entry->entry.curr_line = 0;
    entry->entry.currPos = 0;
    entry->lineNum = 0;

    WidgetConnect(entry, GUI_TRIGGER_MOUSE_PRESS, EntryWidgetPress, NULL);
    WidgetConnect(entry, GUI_TRIGGER_WIDGET_UNFOCUS, EntryWidgetUnfocus, NULL);

    WidgetConnect(entry, GUI_TRIGGER_ENTRY_CHAR_INPUT, EntryWidgetCharInput, NULL);

    WidgetConnect(entry, GUI_TRIGGER_ENTRY_KEY_PRESS_INPUT, EntryAreaWidgetKeyPressInput, NULL);

}
