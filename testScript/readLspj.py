import xml.etree.ElementTree as ET
import json


def getinfo_from_RTFEncoded(rtf_base64Encoded):
    import base64
    RTFtext = base64.b64decode(rtf_base64Encoded).decode('utf-8')
    print(RTFtext)
    redpos= RTFtext.find("\\red")
    greenpos= RTFtext.find("\\green")
    bluepos= RTFtext.find("\\blue")
    end_index = RTFtext.find(";",bluepos)
    red = int(RTFtext[redpos+4: greenpos])
    green = int(RTFtext[greenpos+6: bluepos ])
    blue =int(RTFtext[bluepos+5:end_index])
    fontColor = (blue << 16) | (green << 8) | red

    fontsizeEnd = RTFtext.find("\\lang")
    fontsizeStart = RTFtext.rfind("\\fs")
    fontSize = int(RTFtext[fontsizeStart+3:fontsizeEnd])
    
    print(f"fontSize:{fontSize}")
    print(f"fontColor:{fontColor}")

    from striprtf.striprtf import rtf_to_text
    text_content_parsed = rtf_to_text(RTFtext)
    print(text_content_parsed)

    return fontSize,fontColor,text_content_parsed

def parseLspj(lspjPath):
    # parse the XML file
    tree = ET.parse(lspjPath)
    root = tree.getroot()

    # iterate over all the Area elements and extract their attributes
    for area in root.findall(".//Area"):
        area_no = area.get("AreaNo")
        area_name = area.get("AreaName")
        area_rect_left = area.get("AreaRect_Left")
        area_rect_top = area.get("AreaRect_Top")
        area_rect_right = area.get("AreaRect_Right")
        area_rect_bottom = area.get("AreaRect_Bottom")
        print(f"Area {area_no}: {area_name} ({area_rect_left}, {area_rect_top}, {area_rect_right}, {area_rect_bottom})")

        for element in area.iter():
            if element is not area:
                print(f"child element {element.tag}, {element.text}")
                attrib_dict = element.attrib
                attrib_str = ", ".join([f"{attr}={value}" for attr, value in attrib_dict.items()])
                print(f"  {element.tag}: {attrib_str}")

    areas=[]
    for area in root.findall(".//Area"):
        oneArea={}
        areatype=""
        for attr,value in area.attrib.items():
            oneArea[attr]=value
        for element in area.iter():
            if element is not area:
                subDic={}
                areatype= element.tag
                for attr,value in element.attrib.items():
                    subDic[attr]=value
                subDic["text"]=element.text
                if areatype=="SingleLineArea":
                    fontSize,fontColor,text= getinfo_from_RTFEncoded(element.text)
                    subDic["fontSize"]=fontSize
                    subDic["fontColor"]=fontColor
                    subDic["text"]=text

                oneArea["areaprops"]=subDic
        oneArea['areatype']=areatype
        areas.append(oneArea)
    return areas


if __name__ == "__main__":
    lspjPath = r'D:\Cloud_wu\LED\led_server\lib\dll\mutil_areas.lsprj'
    areas = parseLspj(lspjPath)

    print(json.dumps(areas,indent=4))

    text_c = areas[0]['areaprops']['text']
    print(text_c)

    with open('areas.json', 'w') as f:
        json.dump(areas, f, indent=4)