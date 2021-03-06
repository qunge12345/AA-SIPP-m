#include "aa_sipp.h"

AA_SIPP::AA_SIPP(double weight, int constraints_type, int rescheduling, int timelimit, int prioritization, int startsafeinterval, int tweight)
{
    this->weight = weight;
    this->constraints_type = constraints_type;
    this->rescheduling = rescheduling;
    if(timelimit > 0)
        this->timelimit = timelimit;
    else
        this->timelimit = CN_INFINITY;
    this->prioritization = prioritization;
    this->startsafeinterval = startsafeinterval;
    this->tweight = tweight;
    closeSize = 0;
    openSize = 0;
    constraints = nullptr;
}

AA_SIPP::~AA_SIPP()
{
}

bool AA_SIPP::stopCriterion()
{
    if(openSize == 0)
    {
        std::cout << "OPEN list is empty! ";
        return true;
    }
    return false;
}

double AA_SIPP::getCost(int a_i, int a_j, int b_i, int b_j)
{
    return sqrt((a_i - b_i) * (a_i - b_i) + (a_j - b_j) * (a_j - b_j));
}

double AA_SIPP::calcHeading(const Node &node, const Node &son)
{
    double heading = acos((node.i - son.i)/getCost(son.i, son.j, node.i, node.j))*180/PI;
    if(heading > 180)
        heading = 360 - heading;
    return heading;
}

void AA_SIPP::findSuccessors(const Node curNode, const Map &map, std::list<Node> &succs, int numOfCurAgent)
{
    Node newNode, angleNode;
    std::vector<double> EAT;
    std::vector<std::pair<double, double>> intervals;
    double h_value;
    auto parent = &(close.find(curNode.i*map.width + curNode.j)->second);
    for(int i = -1; i <= +1; i++)
    {
        for(int j = -1; j <= +1; j++)
        {
            if(((i == 0 && j != 0) || (i != 0 && j == 0)) && map.CellOnGrid(curNode.i + i, curNode.j + j) && map.CellIsTraversable(curNode.i + i, curNode.j + j))
            {
                newNode.i = curNode.i + i;
                newNode.j = curNode.j + j;
                newNode.heading = calcHeading(curNode, newNode);
                angleNode = curNode;                                                 //the same state, but with extended g-value
                angleNode.g += fabs(angleNode.heading - newNode.heading)*tweight/180;//to compensate the amount of time required for rotation
                newNode.g = angleNode.g + 1.0;
                newNode.Parent = &angleNode;
                h_value = weight*getCost(newNode.i, newNode.j, map.goal_i[numOfCurAgent], map.goal_j[numOfCurAgent]);
                if(angleNode.g <= angleNode.interval.second)
                {
                    intervals = constraints->findIntervals(newNode, EAT, close, map.width);
                    for(unsigned int k = 0; k < intervals.size(); k++)
                    {
                        newNode.interval = intervals[k];
                        newNode.Parent = parent;
                        newNode.g = EAT[k];
                        newNode.F = newNode.g + h_value;
                        succs.push_front(newNode);
                    }
                }
                newNode = resetParent(newNode, curNode, map);
                if(newNode.Parent->i != parent->i || newNode.Parent->j != parent->j)
                {
                    angleNode = *newNode.Parent;
                    newNode.heading = calcHeading(*newNode.Parent, newNode);//new heading with respect to new parent
                    angleNode.g += fabs(angleNode.heading - newNode.heading)*tweight/180;//count new additional time required for rotation
                    newNode.g += fabs(angleNode.heading - newNode.heading)*tweight/180;
                    newNode.Parent = &angleNode;
                    if(angleNode.g > angleNode.interval.second)
                        continue;
                    intervals = constraints->findIntervals(newNode, EAT, close, map.width);
                    for(unsigned int k = 0; k < intervals.size(); k++)
                    {
                        newNode.interval = intervals[k];
                        newNode.Parent = parent->Parent;
                        newNode.g = EAT[k];
                        newNode.F = newNode.g + h_value;
                        succs.push_front(newNode);
                    }
                }
            }
        }
    }
}

bool AA_SIPP::lineOfSight(int i1, int j1, int i2, int j2, const Map &map)
{
    int delta_i = std::abs(i1 - i2);
    int delta_j = std::abs(j1 - j2);
    int step_i = (i1 < i2 ? 1 : -1);
    int step_j = (j1 < j2 ? 1 : -1);
    int error = 0;
    int i = i1;
    int j = j1;
    int sep_value = delta_i*delta_i + delta_j*delta_j;
    if(delta_i == 0)
    {
        for(; j != j2; j += step_j)
            if(map.CellIsObstacle(i, j))
                return false;
    }
    else if(delta_j == 0)
    {
        for(; i != i2; i += step_i)
            if(map.CellIsObstacle(i, j))
                return false;
    }
    else if(delta_i > delta_j)
    {
        for(; i != i2; i += step_i)
        {
            if(map.CellIsObstacle(i, j))
                return false;
            if(map.CellIsObstacle(i, j + step_j))
                return false;
            error += delta_j;
            if(error > delta_i)
            {
                if(((error << 1) - delta_i - delta_j)*((error << 1) - delta_i - delta_j) < sep_value)
                    if(map.CellIsObstacle(i + step_i, j))
                        return false;
                if((3*delta_i - ((error << 1) - delta_j))*(3*delta_i - ((error << 1) - delta_j)) < sep_value)
                    if(map.CellIsObstacle(i, j + 2*step_j))
                        return false;
                j += step_j;
                error -= delta_i;
            }
        }
        if(map.CellIsObstacle(i, j))
            return false;
        if(map.CellIsObstacle(i, j + step_j))
            return false;
    }
    else
    {
        for(; j != j2; j += step_j)
        {
            if(map.CellIsObstacle(i, j))
                return false;
            if(map.CellIsObstacle(i + step_i, j))
                return false;
            error += delta_i;
            if(error > delta_j)
            {
                if(((error << 1) - delta_i - delta_j)*((error << 1) - delta_i - delta_j) < sep_value)
                    if(map.CellIsObstacle(i, j + step_j))
                        return false;
                if((3*delta_j - ((error << 1) - delta_i))*(3*delta_j - ((error << 1) - delta_i)) < sep_value)
                    if(map.CellIsObstacle(i + 2*step_i, j))
                        return false;
                i += step_i;
                error -= delta_j;
            }
        }
        if(map.CellIsObstacle(i, j))
            return false;
        if(map.CellIsObstacle(i + step_i, j))
            return false;
    }
    return true;
}

Node AA_SIPP::findMin(int size)
{
    Node min;
    min.F = std::numeric_limits<float>::max();
    for(int i = 0; i < size; i++)
    {
        if(open[i].size() != 0 && open[i].begin()->F <= min.F)
        {
            if (open[i].begin()->F == min.F)
            {
                if (open[i].begin()->g >= min.g)
                    min=*open[i].begin();
            }
            else
                min=*open[i].begin();
        }
    }
    return min;
}

void AA_SIPP::addOpen(Node &newNode)
{
    if (open[newNode.i].size() == 0)
    {
        open[newNode.i].push_back(newNode);
        openSize++;
        return;
    }

    std::list<Node>::iterator iter, pos, delpos;
    bool posFound = false;
    pos = open[newNode.i].end();
    for(iter = open[newNode.i].begin(); iter != open[newNode.i].end(); ++iter)
    {
        if ((iter->F >= newNode.F) && (!posFound))
        {
            if (fabs(iter->F - newNode.F) < CN_EPSILON)//CN_EPSILON is needed to prevent mistakes with comparison of double-type values
            {
                if (newNode.g > iter->g)
                {
                    pos = iter;
                    posFound = true;
                }
            }
            else
            {
                pos = iter;
                posFound = true;
            }
        }

        if (iter->j == newNode.j && iter->interval.first == newNode.interval.first)
        {
            if((iter->g - (newNode.g + fabs(newNode.heading - iter->heading)*tweight/180)) < CN_EPSILON)//if existing state dominates new one
                return;
            if((newNode.g - (iter->g + fabs(newNode.heading - iter->heading)*tweight/180)) < CN_EPSILON)//if new state dominates the existing one
            {
                if(pos == iter)
                {
                    iter->F = newNode.F;
                    iter->g = newNode.g;
                    iter->interval = newNode.interval;
                    iter->Parent = newNode.Parent;
                    iter->heading = newNode.heading;
                    return;
                }
                delpos = iter;
                iter--;
                open[newNode.i].erase(delpos);
                openSize--;
            }
        }
    }
    open[newNode.i].insert(pos, newNode);
    openSize++;
    return;
}

void AA_SIPP::setPriorities(const Map& map)
{
    current_priorities.clear();
    current_priorities.resize(map.agents, -1);
    if(prioritization == CN_IP_FIFO)
        for(int i = 0; i < map.agents; i++)
            current_priorities[i] = i;
    else  if(prioritization != CN_IP_RANDOM)
    {
        std::vector<double> dists(map.agents, -1);
        for(int i = 0; i < map.agents; i++)
            dists[i] = sqrt(pow(map.start_i[i] - map.goal_i[i], 2) + pow(map.start_j[i] - map.goal_j[i], 2));
        int k = map.agents - 1;
        while(k >= 0)
        {
            double mindist = CN_INFINITY;
            int min_i = -1;
            for(unsigned int i = 0; i < dists.size(); i++)
                if(mindist > dists[i])
                {
                    min_i = i;
                    mindist = dists[i];
                }
            if(prioritization == CN_IP_LONGESTF)
                current_priorities[k] = min_i;
            else
                current_priorities[map.agents - k - 1] = min_i;
            dists[min_i] = CN_INFINITY;
            k--;
        }
    }
    else //random
    {
        for(int i = 0; i < map.agents; i++)
            current_priorities[i] = i;
        std::mt19937 g(rand());
        std::shuffle(current_priorities.begin(), current_priorities.end(), g);
    }
}

bool AA_SIPP::changePriorities(int bad_i)
{
    if(rescheduling == CN_RE_NO)
        return false;

    priorities.push_back(current_priorities);
    if(rescheduling == CN_RE_RULED) //rises the piority of the agent that can't find its path
    {
        for(auto it = current_priorities.begin(); it != current_priorities.end(); it++)
            if(*it == bad_i)
            {
                current_priorities.erase(it);
                current_priorities.insert(current_priorities.begin(), bad_i);
                break;
            }
        for(unsigned int i = 0; i < priorities.size(); i++)
            for(unsigned int j = 0; j < priorities[i].size(); j++)
            {
                if(j + 1 == priorities[i].size())
                    return false;
                if(current_priorities[j] != priorities[i][j])
                    break;
            }
        return true;
    }
    else //random
    {
        std::mt19937 g(rand());
        std::shuffle(current_priorities.begin(),current_priorities.end(), g);
        bool unique = false;
        int maxtries(1), tries(0);
        for(unsigned int i = 1; i <= current_priorities.size(); i++)
            maxtries *= i;
        while(!unique && tries < maxtries)
        {
            tries++;
            for(unsigned int i = 0; i < priorities.size(); i++)
            {
                for(unsigned int j = 0; j < priorities[i].size(); j++)
                {
                    if(j + 1 == priorities[i].size())
                        unique = false;
                    if(current_priorities[j] != priorities[i][j])
                        break;
                }
                if(!unique)
                {
                    std::shuffle(current_priorities.begin(),current_priorities.end(), g);
                    break;
                }
            }
            unique = true;
        }
        return unique;
    }
}

SearchResult AA_SIPP::startSearch(Map &map)
{

#ifdef __linux__
    timeval begin, end;
    gettimeofday(&begin, NULL);
#else
    LARGE_INTEGER begin, end, freq;
    QueryPerformanceCounter(&begin);
    QueryPerformanceFrequency(&freq);
#endif

    bool solution_found(false);
    int tries(0), bad_i(0);
    double timespent(0);
    priorities.clear();
    open.resize(map.height);
    setPriorities(map);
    do
    {
        if(constraints_type == CN_CT_POINT)
            constraints = new PointConstraints(map.width, map.height, tweight);
        else if(constraints_type == CN_CT_VELOCITY)
            constraints = new VelocityConstraints(map.width, map.height, tweight);
        else
            constraints = new SectionConstraints(map.width, map.height, tweight);
        sresult.pathInfo.clear();
        sresult.agents = map.agents;
        sresult.agentsSolved = 0;
        sresult.pathlength = 0;
        sresult.makespan = 0;

        if(startsafeinterval > 0 && startsafeinterval < CN_INFINITY)
            for(int i = 0; i < map.agents; i++)
                constraints->addStartConstraint(map.start_i[i], map.start_j[i], startsafeinterval);
        else if(startsafeinterval != 0)
            for(int i = 0; i < map.agents; i++)
                map.addConstraint(map.start_i[i], map.start_j[i]);
        for(int numOfCurAgent = 0; numOfCurAgent < map.agents; numOfCurAgent++)
        {
            if(startsafeinterval > 0 && startsafeinterval < CN_INFINITY)
                constraints->removeStartConstraint(map.start_i[current_priorities[numOfCurAgent]], map.start_j[current_priorities[numOfCurAgent]]);
            else if(startsafeinterval != 0)
            {
                map.removeConstraint(map.start_i[current_priorities[numOfCurAgent]], map.start_j[current_priorities[numOfCurAgent]]);
                map.removeConstraint(map.goal_i[current_priorities[numOfCurAgent]], map.goal_j[current_priorities[numOfCurAgent]]);
            }
            if(findPath(current_priorities[numOfCurAgent], map))
                constraints->addConstraints(sresult.pathInfo.back().sections);
            else
            {
                bad_i = current_priorities[numOfCurAgent];
                break;
            }
            if(numOfCurAgent + 1 == map.agents)
                solution_found = true;
        }

        delete constraints;
        tries++;
#ifdef __linux__
    gettimeofday(&end, NULL);
    timespent = (end.tv_sec - begin.tv_sec) + static_cast<double>(end.tv_usec - begin.tv_usec) / 1000000;
#else
    QueryPerformanceCounter(&end);
    timespent = static_cast<double long>(end.QuadPart-begin.QuadPart) / freq.QuadPart;
#endif
        if(timespent > timelimit)
            break;
    } while(changePriorities(bad_i) && !solution_found);
#ifdef __linux__
    gettimeofday(&end, NULL);
    sresult.time = (end.tv_sec - begin.tv_sec) + static_cast<double>(end.tv_usec - begin.tv_usec) / 1000000;
#else
    QueryPerformanceCounter(&end);
    sresult.time = static_cast<double long>(end.QuadPart-begin.QuadPart) / freq.QuadPart;
#endif
    sresult.tries = tries;
    if(sresult.pathfound)
    {
        std::vector<conflict> confs = CheckConflicts();
        for(unsigned int i = 0; i < confs.size(); i++)
            std::cout<<confs[i].i<<" "<<confs[i].j<<" "<<confs[i].g<<" "<<confs[i].agent1<<" "<<confs[i].agent2<<"\n";
    }
    return sresult;
}


Node AA_SIPP::resetParent(Node current, Node Parent, const Map &map)
{
    if(Parent.Parent->interval.first==-1 || (current.i == Parent.Parent->i && current.j == Parent.Parent->j))
        return current;
    if(lineOfSight(Parent.Parent->i, Parent.Parent->j, current.i, current.j, map))
    {
        current.g = Parent.Parent->g + getCost(Parent.Parent->i, Parent.Parent->j, current.i, current.j);
        current.Parent = Parent.Parent;
    }
    return current;
}

bool AA_SIPP::findPath(int numOfCurAgent, const Map &map)
{

#ifdef __linux__
    timeval begin, end;
    gettimeofday(&begin, NULL);
#else
    LARGE_INTEGER begin, end, freq;
    QueryPerformanceCounter(&begin);
    QueryPerformanceFrequency(&freq);
#endif
    close.clear();
    for(unsigned int i = 0; i< open.size(); i++)
        open[i].clear();
    ResultPathInfo resultPath;
    openSize = 0;
    closeSize = 0;

    Node curNode(map.start_i[numOfCurAgent], map.start_j[numOfCurAgent], 0, 0);
    //fake parent for initial orientation
    curNode.i++;
    curNode.interval={-1,-1};
    close.insert({curNode.i * map.width + curNode.j, curNode});
    curNode.Parent = &(close.begin()->second);
    curNode.i--;

    curNode.F = weight * getCost(curNode.i, curNode.j, map.goal_i[numOfCurAgent], map.goal_j[numOfCurAgent]);
    curNode.interval = constraints->getSafeInterval(curNode.i,curNode.j,0);
    curNode.heading = calcHeading(*curNode.Parent, curNode)*tweight/180;
    bool pathFound = false;
    open[curNode.i].push_back(curNode);
    openSize++;
    while(!stopCriterion())
    {
        curNode = findMin(map.height);
        open[curNode.i].pop_front();
        openSize--;
        close.insert({curNode.i * map.width + curNode.j, curNode});
        closeSize++;
        if(curNode.i == map.goal_i[numOfCurAgent] && curNode.j == map.goal_j[numOfCurAgent] && curNode.interval.second == CN_INFINITY)
        {
            pathFound = true;
            break;
        }
        std::list<Node> successors;
        successors.clear();
        findSuccessors(curNode, map, successors, numOfCurAgent);
        for(auto it = successors.begin(); it != successors.end(); it++)
            addOpen(*it);
    }
    if(pathFound)
    {
        makePrimaryPath(curNode);
        for(unsigned int i = 1; i < hppath.size(); i++)
            if((hppath[i].g - (hppath[i - 1].g + getCost(hppath[i].i, hppath[i].j, hppath[i - 1].i, hppath[i - 1].j))) > CN_EPSILON)
            {
                Node add = hppath[i - 1];
                add.Parent = hppath[i].Parent;
                add.g = hppath[i].g - getCost(hppath[i].i, hppath[i].j, hppath[i - 1].i,hppath[i - 1].j);
                auto parent = (close.insert({add.i*map.width + add.j, add}));
                hppath[i].Parent = &(*parent).second;
                hppath.emplace(hppath.begin() + i, add);
                i++;
            }
#ifdef __linux__
        gettimeofday(&end, NULL);
        resultPath.time = (end.tv_sec - begin.tv_sec) + static_cast<double>(end.tv_usec - begin.tv_usec) / 1000000;
#else
        QueryPerformanceCounter(&end);
        resultPath.time = static_cast<double long>(end.QuadPart-begin.QuadPart) / freq.QuadPart;
#endif
        resultPath.sections = hppath;
        makeSecondaryPath(curNode);
        resultPath.nodescreated = openSize + closeSize;
        resultPath.pathfound = true;
        resultPath.path = lppath;
        resultPath.numberofsteps = closeSize;
        resultPath.pathlength = curNode.g;
        sresult.pathfound = true;
        sresult.pathlength += curNode.g;
        sresult.nodescreated += openSize + closeSize;
        sresult.numberofsteps += closeSize;
        sresult.makespan = std::max(sresult.makespan, curNode.g);
        sresult.pathInfo.push_back(resultPath);
        sresult.agentsSolved++;
    }
    else
    {
#ifdef __linux__
        gettimeofday(&end, NULL);
        resultPath.time = (end.tv_sec - begin.tv_sec) + static_cast<double>(end.tv_usec - begin.tv_usec) / 1000000;
#else
        QueryPerformanceCounter(&end);
        resultPath.time = static_cast<double long>(end.QuadPart-begin.QuadPart) / freq.QuadPart;
#endif
        std::cout<<"Path for agent "<<numOfCurAgent<<" not found!\n";
        sresult.pathfound = false;
        sresult.nodescreated += closeSize;
        sresult.numberofsteps += closeSize;
        resultPath.nodescreated = closeSize;
        resultPath.pathfound = false;
        resultPath.path.clear();
        resultPath.sections.clear();
        resultPath.pathlength = 0;
        resultPath.numberofsteps = closeSize;
        sresult.pathInfo.push_back(resultPath);
    }
    return resultPath.pathfound;
}

std::vector<conflict> AA_SIPP::CheckConflicts()
{
    std::vector<conflict> conflicts(0);
    conflict conf;
    Node cur, check;
    std::vector<std::vector<conflict>> positions;
    positions.resize(sresult.agents);
    for(unsigned int i = 0; i < sresult.agents; i++)
    {
        if(!sresult.pathInfo[i].pathfound)
            continue;
        positions[i].resize(0);
        int k = 0;
        double part = 1;
        for(unsigned int j = 1; j < sresult.pathInfo[i].sections.size(); j++)
        {
            cur = sresult.pathInfo[i].sections[j];
            check = sresult.pathInfo[i].sections[j-1];
            int di = cur.i - check.i;
            int dj = cur.j - check.j;
            double dist = (cur.g - check.g)*10;
            int steps = (cur.g - check.g)*10;
            if(dist - steps + part >= 1)
            {
                steps++;
                part = dist - steps;
            }
            else
                part += dist - steps;
            double stepi = double(di)/dist;
            double stepj = double(dj)/dist;
            double curg = double(k)*0.1;
            double curi = check.i + (curg - check.g)*di/(cur.g - check.g);
            double curj = check.j + (curg - check.g)*dj/(cur.g - check.g);
            conf.i = curi;
            conf.j = curj;
            conf.g = curg;
            if(curg <= cur.g)
            {
                positions[i].push_back(conf);
                k++;
            }
            while(curg <= cur.g)
            {
                if(curg + 0.1 > cur.g)
                    break;
                curi += stepi;
                curj += stepj;
                curg += 0.1;
                conf.i = curi;
                conf.j = curj;
                conf.g = curg;
                positions[i].push_back(conf);
                k++;
            }
        }
        if(double(k - 1)*0.1 < sresult.pathInfo[i].sections.back().g)
        {
            conf.i = sresult.pathInfo[i].sections.back().i;
            conf.j = sresult.pathInfo[i].sections.back().j;
            conf.g = sresult.pathInfo[i].sections.back().g;
            positions[i].push_back(conf);
        }
    }
    unsigned int max = 0;
    for(unsigned int i = 0; i < positions.size(); i++)
        if(positions[i].size() > max)
            max = positions[i].size();
    for(unsigned int i = 0; i < sresult.agents; i++)
    {
        for(unsigned int k = 0; k < max; k++)
        {
            for(unsigned int j = i + 1; j < sresult.agents; j++)
            {
                if(!sresult.pathInfo[j].pathfound || !sresult.pathInfo[i].pathfound)
                    continue;
                conflict a, b;
                if(positions[i].size() > k)
                    a = positions[i][k];
                else
                    a = positions[i].back();
                if(positions[j].size() > k)
                    b = positions[j][k];
                else
                    b = positions[j].back();
                if(sqrt((a.i - b.i)*(a.i - b.i) + (a.j - b.j)*(a.j - b.j)) + CN_EPSILON < 1.0)
                {
                    std::cout<<a.i<<" "<<a.j<<" "<<b.i<<" "<<b.j<<" "<<sqrt((a.i - b.i)*(a.i - b.i) + (a.j - b.j)*(a.j - b.j))<<"\n";
                    conf.i = b.i;
                    conf.j = b.j;
                    conf.agent1 = i;
                    conf.agent2 = j;
                    conf.g = b.g;
                    conflicts.push_back(conf);
                }
            }
        }
    }
    return conflicts;
}

void AA_SIPP::makePrimaryPath(Node curNode)
{
    hppath.clear();
    hppath.shrink_to_fit();
    std::list<Node> path;
    path.push_front(curNode);
    if(curNode.Parent->interval.second != -1)
    {
        curNode = *curNode.Parent;
        if(curNode.Parent->interval.second != -1)
        {
            do
            {
                path.push_front(curNode);
                curNode = *curNode.Parent;
            }
            while(curNode.Parent->interval.second != -1);
        }
        path.push_front(curNode);
    }
    for(auto it = path.begin(); it != path.end(); it++)
        hppath.push_back(*it);
    return;
}

void AA_SIPP::makeSecondaryPath(Node curNode)
{
    lppath.clear();
    if(curNode.Parent->interval.second != -1)
    {
        std::vector<Node> lineSegment;
        do
        {
            calculateLineSegment(lineSegment, *curNode.Parent, curNode);
            lppath.insert(lppath.begin(), ++lineSegment.begin(), lineSegment.end());
            curNode = *curNode.Parent;
        }
        while(curNode.Parent->interval.second != -1);
        lppath.push_front(*lineSegment.begin());
    }
    else
        lppath.push_front(curNode);
}

void AA_SIPP::calculateLineSegment(std::vector<Node> &line, const Node &start, const Node &goal)
{
    int i1 = start.i;
    int i2 = goal.i;
    int j1 = start.j;
    int j2 = goal.j;

    int delta_i = std::abs(i1 - i2);
    int delta_j = std::abs(j1 - j2);
    int step_i = (i1 < i2 ? 1 : -1);
    int step_j = (j1 < j2 ? 1 : -1);
    int error = 0;
    int i = i1;
    int j = j1;
    if (delta_i > delta_j)
    {
        for (; i != i2; i += step_i)
        {
            line.push_back(Node(i,j));
            error += delta_j;
            if ((error << 1) > delta_i)
            {
                j += step_j;
                error -= delta_i;
            }
        }
    }
    else
    {
        for (; j != j2; j += step_j)
        {
            line.push_back(Node(i,j));
            error += delta_i;
            if ((error << 1) > delta_j)
            {
                i += step_i;
                error -= delta_j;
            }
        }
    }
    return;
}
