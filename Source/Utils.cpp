#include "Utils.h"


float getMaxLineWidth(const juce::String& text, const juce::Font& font)
{
    juce::StringArray lines;
    lines.addLines(text);
    float maxWidth = 0;
    
    for (auto line : lines)
    {
        float lineWidth = juce::GlyphArrangement::getStringWidth(font, line);
        if (lineWidth > maxWidth)
        {
            maxWidth = lineWidth;
        }
    }
    
    return maxWidth;
}
