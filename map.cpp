#include "map.h"
using namespace tinyxml2;

Map::Map()
{
    height = -1;
    width = -1;
    Grid = 0;
    agents=1;
}
Map::~Map()
{	
    if(Grid)
    {
        for(int i = 0; i < height; i++)
        {
            delete[] Grid[i];
        }

        delete[] Grid;
    }
    delete[] start_i;
    delete[] start_j;
    delete[] goal_i;
    delete[] goal_j;
}

bool Map::getMap(const char* FileName)
{
    const char* grid = 0;
    std::string value;
    XMLElement *root = 0;
    std::string text = "";
    bool hasGrid = false;
    std::stringstream stream;
    XMLDocument doc;
    if(doc.LoadFile(FileName) != XMLError::XML_SUCCESS)
    {
        std::cout << "Error openning input XML file."<<std::endl;
        return false;
    }
    else
        root = doc.FirstChildElement(CNS_TAG_ROOT);
    if (!root)
    {
        std::cout << "No '"<<CNS_TAG_ROOT<<"' element found in XML file."<<std::endl;
        return false;
    }

    XMLElement *map = root->FirstChildElement(CNS_TAG_MAP);
    if (!map)
    {
        std::cout << "No '"<<CNS_TAG_MAP<<"' element found in XML file."<<std::endl;
        return false;
    }

    XMLNode *node = 0;
    XMLElement *element = 0;

    int agentNumber = 0;
    node = map->FirstChild();

    while(node)
    {
        element = node->ToElement();
        value = node->Value();
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);

        if(!hasGrid && height > 0 && width > 0)
        {
            Grid = new int*[height];
            for(int i = 0; i < height; i++)
                Grid[i] = new int[width];
            hasGrid = true;
        }

        if(value == CNS_TAG_HEIGHT)
        {
            text = element->GetText();
            stream<<text;
            stream>>height;
            stream.clear();
            stream.str("");
            if (height <= 0)
            {
                std::cout << "Wrong '"<<CNS_TAG_HEIGHT<<"' value."<<std::endl;
                return false;
            }
        }
        else if(value == CNS_TAG_WIDTH)
        {
            text = element->GetText();
            stream<<text;
            stream>>width;
            stream.clear();
            stream.str("");

            if (width <= 0)
            {
                std::cout << "Wrong '"<<CNS_TAG_WIDTH<<"' value."<<std::endl;
                return false;
            }
        }
        else if(value == CNS_TAG_AGENTS)
        {
            text = element->GetText();
            stream<<text;
            stream>>agents;
            stream.clear();
            stream.str("");

            if (agents <= 0)
            {
                std::cout << "Wrong '"<<CNS_TAG_AGENTS<<"' value\n";
                return false;
            }

            XMLElement *child;
            int countsx(0), countsy(0), countgx(0), countgy(0);
            for(child = map->FirstChildElement(CNS_TAG_SX); child; child = child->NextSiblingElement(CNS_TAG_SX))
                countsx++;
            for(child = map->FirstChildElement(CNS_TAG_SY); child; child = child->NextSiblingElement(CNS_TAG_SY))
                countsy++;
            for(child = map->FirstChildElement(CNS_TAG_FX); child; child = child->NextSiblingElement(CNS_TAG_FX))
                countgx++;
            for(child = map->FirstChildElement(CNS_TAG_FY); child; child = child->NextSiblingElement(CNS_TAG_FY))
                countgy++;
            if(countsx < agents || countsy < agents || countgx < agents || countgy < agents)
            {
                std::cout<<"There is not enough information about "<< agents<<" agents!\n";
                return false;
            }
            start_j = new int[agents];
            start_i = new int[agents];
            goal_j = new int[agents];
            goal_i = new int[agents];
        }
        else if(value == CNS_TAG_SX)
        {
            text = element->GetText();
            stream<<text;
            stream>>start_j[agentNumber];
            stream.clear();
            stream.str("");

            if (start_j[agentNumber] < 0 || start_j[agentNumber] >= width)
            {
                std::cout << "Wrong '"<<CNS_TAG_SX<<"' value."<<std::endl;
                return false;
            }
        }
        else if(value == CNS_TAG_SY)
        {
            text = element->GetText();
            stream<<text;
            stream>>start_i[agentNumber];
            stream.clear();
            stream.str("");

            if (start_i[agentNumber] < 0 || start_i[agentNumber] >= height)
            {
                std::cout << "Wrong '"<<CNS_TAG_SY<<"' value."<<std::endl;
                return false;
            }
        }
        else if(value == CNS_TAG_FX)
        {
            text = element->GetText();
            stream<<text;
            stream>>goal_j[agentNumber];
            stream.clear();
            stream.str("");

            if (goal_j[agentNumber] < 0 || goal_j[agentNumber] >= width)
            {
                std::cout << "Wrong '"<<CNS_TAG_FX<<"' value."<<std::endl;
                return false;
            }
        }
        else if(value == CNS_TAG_FY)
        {
            text = element->GetText();
            stream<<text;
            stream>>goal_i[agentNumber];
            stream.clear();
            stream.str("");

            if (goal_i[agentNumber] < 0 || goal_i[agentNumber] >= height)
            {
                std::cout << "Wrong '"<<CNS_TAG_FY<<"' value."<<std::endl;
                return false;
            }
            agentNumber++;
        }
        else if(value == CNS_TAG_GRID)
        {
            if(height == -1 || width == -1)
            {
                std::cout << "No '"<<CNS_TAG_HEIGHT<<"' or '"<<CNS_TAG_WIDTH<<"' before '"<<CNS_TAG_GRID<<"' given."<<std::endl;
                return false;
            }

            element = node->FirstChildElement(CNS_TAG_ROW);

            int i=0;
            while (i < height)
            {
                if (!element)
                {
                    std::cout << "Not enough '"<<CNS_TAG_ROW<<"' in '"<<CNS_TAG_GRID<<"' given."<<std::endl;
                    return false;
                }

                grid = element->GetText();
                unsigned int k = 0;
                text = "";
                int j = 0;

                for(k = 0; k < (strlen(grid)); k++)
                {
                    if (grid[k] == ' ')
                    {
                        stream<<text;
                        stream>>Grid[i][j];
                        stream.clear();
                        stream.str("");
                        text = "";
                        j++;
                    }
                    else
                    {
                        text += grid[k];
                    }
                }
                stream<<text;
                stream>>Grid[i][j];
                stream.clear();
                stream.str("");

                if (j < width-1)
                {
                    std::cout << "Not enough cells in '"<<CNS_TAG_ROW<<"' "<< i <<" given."<<std::endl;
                    return false;
                }

                i++;
                element = element->NextSiblingElement();
            }
        }
        node = node->NextSibling();
    }
    return true;
}


bool Map::CellIsTraversable(int i, int j) const
{
    return (Grid[i][j] == 0);
}

bool Map::CellIsObstacle(int i, int j) const
{
    return (Grid[i][j] != 0);
}

bool Map::CellOnGrid(int i, int j) const
{
    return (i < height && i >= 0 && j < width && j >= 0);
}

int Map::getValue(int i, int j) const
{
    if(i<0 || i>=height)
        return -1;

    if(j<0 || j>=width)
        return -1;

    return Grid[i][j];
}


void Map::addConstraint(int i, int j)
{
    Grid[i][j]=CN_OBSTL;
}

void Map::removeConstraint(int i, int j)
{
    Grid[i][j]=0;
}
