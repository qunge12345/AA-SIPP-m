#ifndef GL_CONST_H
#define GL_CONST_H

#define PI              3.14159265359
#define CN_EPSILON      1e-5
#define CN_INFINITY		1000000
#define CN_LOG          "_log"

//Parameters Type
#define CN_PT_LOGLVL    0   // loglevel
#define CN_PT_WEIGHT    1   // weight
#define CN_PT_MT        2   // metrictype
#define CN_PT_AA        3   // allow any-angle
#define CN_PT_CT        4   // constraints type
#define CN_PT_TL        5   // time limit
#define CN_PT_RE        6   // rescheduling
#define CN_PT_IP        7   // initial prioritization
#define CN_PT_SSF       8   // start safe interval
#define CN_PT_TW        9   // turning weight

#define CN_PT_NUM       10   //number of parameters


//Obstacle
#define CN_OBSTL 1

//loglevel
#define CN_LOGLVL_NO	0
#define CN_LOGLVL_HIGH	1

//breakingties
#define CN_BT_G_MAX     1
#define CN_BT_G_MIN     2

//metrictype
#define CN_MT_EUCLID        1
#define CN_MT_DIAGONAL      2
#define CN_MT_MANHATTAN     3

#define CNS_MT_EUCLID       "euclid"
#define CNS_MT_DIAGONAL     "diagonal"
#define CNS_MT_MANHATTAN    "manhattan"

//constraints_type
#define CN_CT_POINT     1
#define CN_CT_VELOCITY  2
#define CN_CT_SECTION   3

#define CNS_CT_POINT     "point"
#define CNS_CT_VELOCITY  "velocity"
#define CNS_CT_SECTION   "section"

//initial prioritization
#define CN_IP_SHORTESTF  1
#define CN_IP_LONGESTF   2
#define CN_IP_RANDOM     3
#define CN_IP_FIFO       4

#define CNS_IP_SHORTESTF "shortest_first"
#define CNS_IP_LONGESTF  "longest_first"
#define CNS_IP_RANDOM    "random"
#define CNS_IP_FIFO      "fifo"

//rescheduling
#define CN_RE_NO         1
#define CN_RE_RULED      2
#define CN_RE_RANDOM     3

#define CNS_RE_NO        "no"
#define CNS_RE_RULED     "rulebased"
#define CNS_RE_RANDOM    "random"

/*
 * XML file tags ---------------------------------------------------------------
 */
#define CNS_TAG_ROOT "root"
    #define CNS_TAG_ALGORITHM           "algorithm"
    #define CNS_TAG_MAP                 "map"
    #define CNS_TAG_AGENTS              "agents"
    #define CNS_TAG_HEIGHT              "height"
    #define CNS_TAG_WIDTH               "width"
    #define CNS_TAG_SX                  "startx"
    #define CNS_TAG_SY                  "starty"
    #define CNS_TAG_FX                  "finishx"
    #define CNS_TAG_FY                  "finishy"
    #define CNS_TAG_GRID                "grid"
        #define CNS_TAG_ROW             "row"
    #define CNS_TAG_WEIGHT              "weight"
    #define CNS_TAG_HWEIGHT             "hweight"
    #define CNS_TAG_ALLOW_AA            "allowanyangle"
    #define CNS_TAG_PRIORITIZATION      "prioritization"
    #define CNS_TAG_RESCHEDULING        "rescheduling"
    #define CNS_TAG_STARTSAFEINTERVAL   "startsafeinterval"
    #define CNS_TAG_TIMELIMIT           "timelimit"
    #define CNS_TAG_METRICTYPE          "metrictype"
    #define CNS_TAG_CONSTRAINTSTYPE     "constraintstype"
    #define CNS_TAG_TURNINGWEIGHT       "turningweight"
    #define CNS_TAG_OPTIONS             "options"
    #define CNS_TAG_LOGLVL              "loglevel"
    #define CNS_TAG_LOG                 "log"
        #define CNS_TAG_MAPFN           "mapfilename"
        #define CNS_TAG_SUM             "summary"
        #define CNS_TAG_PATH            "path"
        #define CNS_TAG_ROW             "row"
        #define CNS_TAG_LPLEVEL         "lplevel"
        #define CNS_TAG_HPLEVEL         "hplevel"
        #define CNS_TAG_AGENT           "agent"
        #define CNS_TAG_LOWLEVEL        "lowlevel"
            #define CNS_TAG_SECTION     "section"
            #define CNS_TAG_STEP        "step"
            #define CNS_TAG_OPEN        "open"
            #define CNS_TAG_NODE        "node"
            #define CNS_TAG_CLOSE       "close"

/*
 * End of XML files tags -------------------------------------------------------
 */

/*
 * XML files tag's attributes --------------------------------------------------
 */
    #define CNS_TAG_ATTR_NUMOFSTEPS     "numberofsteps"
    #define CNS_TAG_ATTR_TOTALNODES     "totalnodescreated"
    #define CNS_TAG_ATTR_NODES          "nodescreated"
    #define CNS_TAG_ATTR_LENGTH         "length"
    #define CNS_TAG_ATTR_PATHLENGTH     "pathlength"
    #define CNS_TAG_ATTR_TIME           "time"
    #define CNS_TAG_ATTR_X              "x"
    #define CNS_TAG_ATTR_Y              "y"
    #define CNS_TAG_ATTR_NUM            "number"
    #define CNS_TAG_ATTR_F              "F"
    #define CNS_TAG_ATTR_G              "g"
    #define CNS_TAG_ATTR_PARX           "parent_x"
    #define CNS_TAG_ATTR_PARY           "parent_y"
    #define CNS_TAG_ATTR_VALUE          "value"
    #define CNS_TAG_ATTR_SX             "start.x"
    #define CNS_TAG_ATTR_SY             "start.y"
    #define CNS_TAG_ATTR_FX             "finish.x"
    #define CNS_TAG_ATTR_FY             "finish.y"
    #define CNS_TAG_ATTR_PF             "pathfound"
    #define CNS_TAG_ATTR_SOLVED         "solved"
        #define CNS_TAG_ATTR_TRUE       "true"
        #define CNS_TAG_ATTR_FALSE      "false"
    #define CNS_TAG_ATTR_TRIES          "tries"
    #define CNS_TAG_ATTR_AGENTSSOLVED   "agentssolved"
    #define CNS_TAG_ATTR_PATHSFOUND     "pathsfound"
    #define CNS_TAG_ATTR_PATHFOUND      "pathfound"
    #define CNS_TAG_ATTR_MAXNODESCR     "maxnodescreated"
    #define CNS_TAG_ATTR_FLOWTIME       "flowtime"
    #define CNS_TAG_ATTR_AVGLENGTH      "avglength"
    #define CNS_TAG_ATTR_MAKESPAN       "makespan"

/*
 * End of XML files tag's attributes -------------------------------------------
 */

#define CN_NO_DIR        0
#define CN_LEFT_DIR      1
#define CN_UP_DIR        2
#define CN_RIGHT_DIR     3
#define CN_DOWN_DIR      4
#define CN_GOAL_DIR      5

#define CN_PARALLEL          1
#define CN_COINCIDENT        2
#define CN_NONINTERSECTING   3
#define CN_INTERSECTING      4



#endif
