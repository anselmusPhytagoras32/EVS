# Building Evacuation Plan Designer and Simulator

## Target Problem
The challenge is the absence of validated, site-based evacuation plans for Small-to-Medium Enterprises (SMEs) and schools in the seismic-risk National Capital Region (NCR). The Philippines is one of the most exposed to natural disaster risk countries globally, with high seismic threats since it is located on the "Ring of Fire" [(Global Facility for Disaster Reduction and Recovery, 2016)](https://www.gfdrr.org/en/feature-story/protecting-lives-and-property-disaster-risk-philippines). Existing plans tend to be generic and not proven against actual disaster scenarios, such as jammed primary exits or structural failure due to a severe ground shaking. Such a severe shortfall makes occupants extremely vulnerable, and only 20% of Filipinos have taken the initiative to develop their own disaster management plan [(Harvard Humanitarian Initiative, 2024)] (https://reliefweb.int/report/philippines/republic-philippines-survey-disaster-preparedness-and-climate-change-perceptions-06-november-2024). The problem is in the Preparedness stage, where there is a need for an available tool to generate dynamic, tested escape plans that eliminate panic and provide the fastest, safest route.

## Existing Software Solutions

1. **drawio**
Multipurpose diagram tool for software, business and architectural diagrams, it can also be used for making evacuation plans. It is fast, versatile and easy to use. However, it can only be used to create diagrams, Our project, while only focused on creating floor plans and designating evacuation routes, will have a feature to evaluate the effectiveness of an evacuation plan. 
[(draw.io LTD, 2005)] (https://www.drawio.com/index.html)

2. **Room Sketcher**
Floor plan designer for creating floor plans with either 2d or 3d visuals. Offers many features for architectural design and available on desktop and mobile platforms except for linux. Like the previous software, it does not have a feature for simulating and evaluating evacuation plans. [(RoomSketcher, 2007)] (https://www.roomsketcher.com/)

## Proposed Solution

1. **Evacuation Plan Simulation and Evaluation**
Simulates how the evacuation plan will play out in various disaster scenarios. Data on foot traffic and travel time to emergency exits will be measured whilst a disaster threatens the evacuees and hinder their evacuation. During a simulation, a specified amount of people will attempt to travel towards emergency exits using paths and areas designated by the user.

2. **Evacuation Plan Designer**
Allow users to design an evacuation plan from scratch; creating
a floor plan then designating emergency exits and areas where
evacuees can traverse through. Alternatively, the user can import
an existing floor plan and add designations to it.

```
---
config:
  theme: forest
  layout: dagre
---
flowchart LR
    A(["Start"]) --> L{"Import Existing Plan?"}
    L --> M["Yes"] & N["No"]
    M --> O["Get Existing Plan From File"]
    O --> P["Get Image Of Building Layout"]
    N --> P
    P --> B["User Choice"]
    B --> C["Add New Room"] & D["Edit Existing Room"] & E["Delete Room"] & F["Run Simulation"] & Q["Save Current Evacuation Plan"] & R["End Program"]
    C --> G["Get New Room Info"]
    D --> H["Get Room To Edit And Update Room Info"]
    E --> J["Get Room To Delete"]
    F --> K["Run Simulation Using Data Provided By User"]
    G --> B
    H --> B
    J --> B
    K --> B
    Q --> B
    B@{ shape: diam}
```

## Application of Data Structures
A dynamically allocated stack will be used for storing rooms and other designations in the floor plan when rendering during floor plan creation. A stack will make it quicker to access recently added designations in case the user wants to undo a change. A modified tree will be used to sort rooms when simulating an evacuation plan. Each designated exit will serve as a root node and its descendants will be arranged in terms of distance from the exit, rooms that directly lead to the exit will be the children of the root node and the furthest rooms will be the leaf nodes of that tree. This is to simulate evacuees moving from room to room until they reach the exit.


## References
Global Facility for Disaster Reduction and Recovery. (2016, June 27). Protecting lives and property from disaster risk in the Philippines. The World Bank. https://www.gfdrr.org/en/feature-story/protecting-lives-and-property-disaster-risk-philippines
Climate Change Commission. (2011, September 30). Disaster preparedness and first aid handbook. https://climate.gov.ph/files/Disaster_Preparedness_First_Aid_Handbook.pdf

Harvard Humanitarian Initiative. (2024, November 6). Republic of the Philippines: Survey on disaster preparedness and climate change perceptions, 06 November 2024. ReliefWeb. https://reliefweb.int/report/philippines/republic-philippines-survey-disaster-preparedness-and-climate-change-perceptions-06-november-2024

## Authors
Aaron Alexander Beard
Manuel Miko Delola
Gebhel Anselm L. Santos
