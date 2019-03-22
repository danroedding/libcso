#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cso.h>

int serialize(container *obj, char **buf, int *buflen)
{
  int ret=cso_serialize(obj,buf,buflen);

  if(ret>=0) {
    printf("Serialized container (size: %d, strlen: %d):\n\n%s\n\n",
      *buflen,(int)strlen(*buf),*buf);
    free(*buf);
    return 0;
  } else {
    printf("Couldn't serialize container...\n");
    cso_delete(obj);
    if(*buf!=NULL) free(*buf);
    return 1;
  }
}

int main(void)
{
  container *obj=NULL;
  char *buf=NULL;
  int buflen=0;
  int ret=0;

  /**
   *  new & delete
   */
  printf("Creating test container... ");
  obj = cso_new(NULL);

  if(obj==NULL) {
    printf("\nCouldn't create container... aborting!\n");
    return 1;
  }

  printf("success\n"
         "Deleting test container... ");

  cso_delete(obj);

  printf("success\nCreating work container... ");
  obj = cso_new("container_1");

  if(obj==NULL) {
    printf("\nCouldn't create container... aborting!\n");
    return 1;
  }

  printf("success\n");

  /**
   *  setter & getter for string
   */

  printf("Setting some string values...\n\n");

  {
    const char *field="string_1";
    const char *value="A string value";
    ret=cso_set_str(obj,field,0,value);

    if(ret!=0) {
      printf("Error while setting string field \"%s\": %d\n",
        field,ret);
      cso_delete(obj);
      return 1;
    } else {
      printf("Success, field: \"%s\", value: \"%s\"\n",
        field,value);

      const char *ptr=cso_get_str(obj,field,0);

      if(ptr) {
        printf("Retrieved value of field \"%s\": \"%s\"\n",field,ptr);
      } else {
        printf("Couldn't retrieve value of field \"%s\"... aborting!\n",field);
        cso_delete(obj);
        return 1;
      }
    }

    printf("\n");
  }

  {
    const char *field="string_2";
    const char *value="A long time ago in a galaxy far, far away...";
    ret=cso_set_str(obj,field,0,value);

    if(ret!=0) {
      printf("Error while setting string field \"%s\": %d\n",
        field,ret);
      cso_delete(obj);
      return 1;
    } else {
      printf("Success, field: \"%s\", value: \"%s\"\n",
        field,value);

      const char *ptr=cso_get_str(obj,field,0);

      if(ptr) {
        printf("Retrieved value of field \"%s\": \"%s\"\n",field,ptr);
      } else {
        printf("Couldn't retrieve value of field \"%s\"... aborting!\n",field);
        cso_delete(obj);
        return 1;
      }
    }

    printf("\n");
  }

  {
    const char *field="string_3";
    const char *value="123456789";
    ret=cso_set_str(obj,field,0,value);

    if(ret!=0) {
      printf("Error while setting string field \"%s\": %d\n",
        field,ret);
      cso_delete(obj);
      return 1;
    } else {
      printf("Success, field: \"%s\", value: \"%s\"\n",
        field,value);

      const char *ptr=cso_get_str(obj,field,0);

      if(ptr) {
        printf("Retrieved value of field \"%s\": \"%s\"\n",field,ptr);
      } else {
        printf("Couldn't retrieve value of field \"%s\"... aborting!\n",field);
        cso_delete(obj);
        return 1;
      }

      int val=cso_get_int(obj,field,0);

      if(val!=0) {
        printf("Retrieved (int) value of field \"%s\": %d\n",field,val);
      } else {
        printf("Couldn't retrieve (int) value of field \"%s\"... aborting!\n",field);
        cso_delete(obj);
        return 1;
      }
    }

    printf("\n");
  }

  /**
   *  serialize
   */
  if(serialize(obj,&buf,&buflen)) {
    cso_delete(obj);
    return 1;
  }

  /**
   *  setter & getter for int
   */
  printf("Setting some int values...\n\n");

  {
    const char *field="int_1";
    int value=42;
    ret=cso_set_int(obj,field,0,value);

    if(ret!=0) {
      printf("Error while setting int field \"%s\": %d\n",
        field,ret);
      cso_delete(obj);
      return 1;
    } else {
      printf("Success, field: \"%s\", value: %d\n",
        field,value);

      int val=cso_get_int(obj,field,0);

      if(val!=0) {
        printf("Retrieved (int) value of field \"%s\": %d\n",field,val);
      } else {
        printf("Couldn't retrieve (int) value of field \"%s\"... aborting!\n",field);
        cso_delete(obj);
        return 1;
      }

      const char *ptr=cso_get_str(obj,field,0);

      if(ptr) {
        printf("Retrieved (string) value of field \"%s\": \"%s\"\n",field,ptr);
      } else {
        printf("Couldn't retrieve (string) value of field \"%s\"... aborting!\n",field);
        cso_delete(obj);
        return 1;
      }
    }

    printf("\n");
  }

  {
    const char *field="int_2";
    int value=123456789;
    ret=cso_set_int(obj,field,0,value);

    if(ret!=0) {
      printf("Error while setting int field \"%s\": %d\n",
        field,ret);
      cso_delete(obj);
      return 1;
    } else {
      printf("Success, field: \"%s\", value: %d\n",
        field,value);

      int val=cso_get_int(obj,field,0);

      if(val!=0) {
        printf("Retrieved (int) value of field \"%s\": %d\n",field,val);
      } else {
        printf("Couldn't retrieve (int) value of field \"%s\"... aborting!\n",field);
        cso_delete(obj);
        return 1;
      }
    }

    printf("\n");
  }

  /**
   *  setter & getter for obj
   */
  printf("Setting some obj values...\n\n");

  {

    container *sub_obj=cso_new("sub_obj");
    cso_set_int(sub_obj,"einezahl",0,1337);
    cso_set_str(sub_obj,"einstring",0,"dies ist ein unterobjekt");

    const char *field="obj_1";
    container *obj_1=cso_new(field);
    cso_set_str(obj_1,"container_name",0,field);
    cso_set_str(obj_1,"content",0, "this container was copied");
    cso_set_obj(obj_1,"sub_obj",0,sub_obj,1);
    ret=cso_set_obj(obj,field,0,obj_1,0);
    cso_delete(obj_1);

    if(ret!=0) {
      printf("Error while setting obj field \"%s\": %d... aborting!\n",
        field,ret);
      cso_delete(obj);
      return 1;
    } else {
      printf("Success, obj field: \"%s\"",field);

      container *tmp_obj=cso_get_obj(obj,field,0,0);

      if(tmp_obj!=NULL) {
        printf("Success, retrieved (obj) field \"%s\" from parent obj\n",field);
      } else {
        printf("Couldn't retrieve (obj) value of field \"%s\"... aborting!\n",field);
        cso_delete(obj);
        return 1;
      }
    }

    printf("\n");
  }

  {
    const char *field="obj_2";
    container *obj_2=cso_new("obj_2");

    cso_set_str(obj_2,"container_name",0,field);
    cso_set_str(obj_2,"content",0, "this container was inherited");
    ret=cso_set_obj(obj,field,0,obj_2,1);

    if(ret!=0) {
      printf("Error while setting obj field \"%s\": %d... aborting!\n",
        field,ret);
      cso_delete(obj_2);
      cso_delete(obj);
      return 1;
    } else {
      printf("Success, obj field: \"%s\"",field);

      if(serialize(obj,&buf,&buflen)) {
        cso_delete(obj);
        return 1;
      }

      printf("Detaching...\n");

      container *tmp_obj=cso_get_obj(obj,field,0,1);

      if(tmp_obj!=NULL) {
        printf("Success, retrieved and dettached (obj) field \"%s\" from parent obj\n",field);
        cso_delete(tmp_obj);
      } else {
        printf("Couldn't retrieve (obj) value of field \"%s\"... aborting!\n",field);
        cso_delete(obj);
        return 1;
      }
    }

    printf("\n");
  }

  /**
   *  serialize
   */
  if(serialize(obj,&buf,&buflen)) {
    cso_delete(obj);
    return 1;
  }

  /**
   *  setter & getter for binaries
   */

  printf("Setting some binary values...\n\n");

  {
    const char *field="bin_1";
    char value[10]="123456789";
    int len=strlen(value);
    ret=cso_set_bin(obj,field,value,len,0);

    if(ret!=0) {
      printf("Error while setting binary field \"%s\": %d\n",
        field,ret);
      cso_delete(obj);
      return 1;
    } else {
      printf("Success, field: \"%s\", value: \"%s\", len: %d\n",
        field,value,len);

      char *retData=NULL;
      int retLen=0;

      ret=cso_get_bin(obj,field,0,&retData,&retLen);

      if(ret==0) {
        char *retVal=(char *)malloc(retLen+1);
        strncpy(retVal,retData,retLen);
        retVal[retLen]='\0';

        printf("Retrieved value of binary field \"%s\" with len %d: \"%s\"\n",field,retLen,retVal);

        free(retVal);
      } else {
        printf("Couldn't retrieve value of field \"%s\"... aborting!\n",field);
        cso_delete(obj);
        return 1;
      }
    }

    printf("\n");
  }

  {
    cso_set_str(obj,"string2binary",0,"Teststring");

    char *binval;
    int binval_len;

    if (cso_get_bin(obj,"string2binary",0,&binval,&binval_len)) {
      printf("String-to-binary conversion failed, cannot retrieve value\n");
      cso_delete(obj);
      return 1;
    }

    if (binval==NULL || binval_len!=strlen("Teststring") ||
        memcmp(binval,"Teststring",strlen("Teststring"))) {
      printf("String-to-binary conversion failed, got wrong value\n");
      cso_delete(obj);
      return 1;
    }

    printf("String-to-binary conversion test successful.\n\n");
  }

  {
    const char *field="bin_2";
    char value[65]="A longer text for the binary field object within this container!";
    int len=strlen(value);
    ret=cso_set_bin(obj,field,value,len,0);

    if(ret!=0) {
      printf("Error while setting bin field \"%s\": %d\n",
        field,ret);
      cso_delete(obj);
      return 1;
    } else {
      printf("Success, field: \"%s\", value: \"%s\", len: %d\n",
        field,value,len);

      char *retData=NULL;
      int retLen=0;

      ret=cso_get_bin(obj,field,0,&retData,&retLen);

      if(ret==0) {
        char *retVal=(char *)malloc(retLen+1);
        strncpy(retVal,retData,retLen);
        retVal[retLen]='\0';

        printf("Retrieved value of binary field \"%s\" with len %d: \"%s\"\n",field,retLen,retVal);

        free(retVal);
      } else {
        printf("Couldn't retrieve value of field \"%s\"... aborting!\n",field);
        cso_delete(obj);
        return 1;
      }
    }

    printf("\n");
  }

  /**
   *  get & set fields by path
   */
  {
    printf("Get & set field by path...\n");

    const char *field="obj_1.sub_obj.einstring";
    const char *ptr=cso_get_str(obj,field,0);

    if(ptr) {
      printf("Retrieved value of field \"%s\": \"%s\"\n",field,ptr);
    } else {
      printf("Couldn't retrieve value of field \"%s\"... aborting!\n",field);
      cso_delete(obj);
      return 1;
    }

    printf("Setting the value of field \"%s\"...\n",field);

    ret=cso_set_str(obj,field,0,"nun ein anderer string");

    if(ret!=0) {
      printf("Error while setting string field \"%s\": %d\n",
        field,ret);
      cso_delete(obj);
      return 1;
    }

    ptr=cso_get_str(obj,field,0);

    if(ptr) {
      printf("Retrieved value of field \"%s\": \"%s\"\n",field,ptr);
    } else {
      printf("Couldn't retrieve value of field \"%s\"... aborting!\n",field);
      cso_delete(obj);
      return 1;
    }

    printf("\n");
  }

  {
    const char *field="obj_1.sub_obj.neuer_string";
    printf("Setting the value of field \"%s\"...\n",field);

    ret=cso_set_str(obj,field,0,"nun ein neu angelegter string");

    if(ret!=0) {
      printf("Error while setting string field \"%s\": %d\n",
        field,ret);
      cso_delete(obj);
      return 1;
    }
  }

  {
    const char *field="obj_1.neues_obj.neuer_int";
    printf("Setting the value of field \"%s\"...\n",field);

    ret=cso_set_int(obj,field,0,1234);

    if(ret!=0) {
      printf("Error while setting string field \"%s\": %d\n",
        field,ret);
      cso_delete(obj);
      return 1;
    }
  }
  
  /**
   *  get types
   */
  printf("\nGetting field types...\n");

  {
    const char *field="string_1";
    cso_fieldtype	type=cso_get_type(obj,field);

    switch(type) {
      case CSO_STRING:
        printf("Success, field \"%s\" has type CSO_STRING\n", field);
        break;
      case CSO_INT:
        printf("Failed, field \"%s\" has type CSO_INT...\n", field);
        break;
      case CSO_OBJECT:
        printf("Failed, field \"%s\" has type CSO_OBJECT...\n", field);
        break;
      case CSO_BINARY:
        printf("Failed, field \"%s\" has type CSO_BINARY...\n", field);
        break;
      case CSO_NONE:
        printf("Failed, field \"%s\" has type CSO_NONE (non-existent)...\n", field);
        break;
      default:
        printf("Failed, couldn't retrieve type for field \"%s\"...\n", field);
    }
  }

  {
    const char *field="int_2";
    cso_fieldtype	type=cso_get_type(obj,field);

    switch(type) {
      case CSO_STRING:
        printf("Failed, field \"%s\" has type CSO_STRING...\n", field);
        break;
      case CSO_INT:
        printf("Success, field \"%s\" has type CSO_INT\n", field);
        break;
      case CSO_OBJECT:
        printf("Failed, field \"%s\" has type CSO_OBJECT...\n", field);
        break;
      case CSO_BINARY:
        printf("Failed, field \"%s\" has type CSO_BINARY...\n", field);
        break;
      case CSO_NONE:
        printf("Failed, field \"%s\" has type CSO_NONE (non-existent)...\n", field);
        break;
      default:
        printf("Failed, couldn't retrieve type for field \"%s\"...\n", field);
    }
  }

  {
    const char *field="obj_1";
    cso_fieldtype	type=cso_get_type(obj,field);

    switch(type) {
      case CSO_STRING:
        printf("Failed, field \"%s\" has type CSO_STRING...\n", field);
        break;
      case CSO_INT:
        printf("Failed, field \"%s\" has type CSO_INT...\n", field);
        break;
      case CSO_OBJECT:
        printf("Success, field \"%s\" has type CSO_OBJECT\n", field);
        break;
      case CSO_BINARY:
        printf("Failed, field \"%s\" has type CSO_BINARY...\n", field);
        break;
      case CSO_NONE:
        printf("Failed, field \"%s\" has type CSO_NONE (non-existent)...\n", field);
        break;
      default:
        printf("Failed, couldn't retrieve type for field \"%s\"...\n", field);
    }
  }

  {
    const char *field="bin_1";
    cso_fieldtype	type=cso_get_type(obj,field);

    switch(type) {
      case CSO_STRING:
        printf("Failed, field \"%s\" has type CSO_STRING...\n", field);
        break;
      case CSO_INT:
        printf("Failed, field \"%s\" has type CSO_INT...\n", field);
        break;
      case CSO_OBJECT:
        printf("Failed, field \"%s\" has type CSO_OBJECT\n", field);
        break;
      case CSO_BINARY:
        printf("Success, field \"%s\" has type CSO_BINARY...\n", field);
        break;
      case CSO_NONE:
        printf("Failed, field \"%s\" has type CSO_NONE (non-existent)...\n", field);
        break;
      default:
        printf("Failed, couldn't retrieve type for field \"%s\"...\n", field);
    }
  }

  {
    const char *field="abc_123";
    cso_fieldtype	type=cso_get_type(obj,field);

    switch(type) {
      case CSO_STRING:
        printf("Failed, field \"%s\" has type CSO_STRING...\n", field);
        break;
      case CSO_INT:
        printf("Failed, field \"%s\" has type CSO_INT...\n", field);
        break;
      case CSO_OBJECT:
        printf("Failed, field \"%s\" has type CSO_OBJECT...\n", field);
        break;
      case CSO_BINARY:
        printf("Failed, field \"%s\" has type CSO_BINARY...\n", field);
        break;
      case CSO_NONE:
        printf("Success, field \"%s\" has type CSO_NONE (non-existent)\n", field);
        break;
      default:
        printf("Failed, couldn't retrieve type for field \"%s\"...\n", field);
    }
  }

  printf("\n");

  /**
   *  field type errors
   */
  {
    const char *field="int_1";
    const char *value="some string value";
    ret=cso_set_str(obj,field,0,value);

    if(ret==1) {
      printf("Success, field type error for field \"%s\"\n",
        field);

      int val=cso_get_int(obj,field,0);

      if(val!=0) {
        printf("Retrieved (int) value of field \"%s\": %d\n",field,val);
      } else {
        printf("Couldn't retrieve (int) value of field \"%s\"... aborting!\n",field);
        cso_delete(obj);
        return 1;
      }
    } else {
      printf("Error while setting int field \"%s\": %d\n",
        field,ret);
      cso_delete(obj);
      return 1;      
    }

    printf("\n");
  }

  {
    const char *field="string_3";
    int value=42;
    ret=cso_set_int(obj,field,0,value);

    if(ret!=0) {
      printf("Error while setting string field \"%s\": %d\n",
        field,ret);
      cso_delete(obj);
      return 1;
    } else {
      printf("Success (no field type error), field: \"%s\", value: \"%d\"\n",
        field,value);

      const char *ptr=cso_get_str(obj,field,0);

      if(ptr) {
        printf("Retrieved (string) value of field \"%s\": \"%s\"\n",field,ptr);
      } else {
        printf("Couldn't retrieve (string) value of field \"%s\"... aborting!\n",field);
        cso_delete(obj);
        return 1;
      }

      int val=cso_get_int(obj,field,0);

      if(val!=0) {
        printf("Retrieved (int) value of field \"%s\": %d\n",field,val);
      } else {
        printf("Couldn't retrieve (int) value of field \"%s\"... aborting!\n",field);
        cso_delete(obj);
        return 1;
      }
    }

    printf("\n");
  }

  /**
   *  serialize
   */
  if(serialize(obj,&buf,&buflen)) {
    cso_delete(obj);
    return 1;
  }

  /**
   *  del
   */
  printf("Deleting some fields...\n");

  {
    const char *field="string_3";
    ret=cso_del(obj,field);

    if(ret!=0) {
      printf("Couldn't delete field \"%s\", error: %d... aborting!",
        field,ret);
      cso_delete(obj);
      return 1;
    } else {
      printf("Successfully delete field \"%s\"\n",
        field);
    }
  }

  {
    const char *field="int_1";
    ret=cso_del(obj,field);

    if(ret!=0) {
      printf("Couldn't delete field \"%s\", error: %d",
        field,ret);
      cso_delete(obj);
      return 1;
    } else {
      printf("Successfully delete field \"%s\"\n",
        field);
    }
  }

  {
    const char *field="bin_2";
    ret=cso_del(obj,field);

    if(ret!=0) {
      printf("Couldn't delete field \"%s\", error: %d",
        field,ret);
      cso_delete(obj);
      return 1;
    } else {
      printf("Successfully delete field \"%s\"\n",
        field);
    }
  }

  {
    const char *field="abc_field";
    ret=cso_del(obj,field);

    if(ret==1) {
      printf("Success, couldn't delete non-existent field \"%s\"\n",
        field);
    } else {
      if(ret==0) {
        printf("Deleted non-existent field \"%s\"... aborting!",
          field);
      } else {
        printf("Some error occured while deleting non-existent field \"%s\", error: %d... aborting!\n",
          field,ret);
      }

      cso_delete(obj);
      return 1;
    }
  }

  printf("\n");

  /*
   *  serialize
   */
  if(serialize(obj,&buf,&buflen)) {
    cso_delete(obj);
    return 1;
  }

  /**
   * copy & delall 
   */
  printf("Copying object... ");

  container *dst=cso_new("copy_obj");
  ret=cso_copy(dst,obj);

  if(ret==0 && dst!=NULL) {
    printf("success\n"
           "Copied object:\n");

    if(serialize(dst,&buf,&buflen)) {
      cso_delete(dst);
      cso_delete(obj);
      return 1;
    }

    printf("Deleting all fields within copy...\n");
    cso_delall(dst);
    printf("Copy-Container should be empty now:\n");

    if(serialize(dst,&buf,&buflen)) {
      cso_delete(dst);
      cso_delete(obj);
      return 1;
    }

    cso_delete(dst);
  } else {
    printf("not successful, error: %d... aborting!\n",
      ret);
    cso_delete(dst);
    cso_delete(obj);
    return 1;
  }

  printf("\n");

  /**
   * arrays
   */
  {
    const char *field="string_1";
    const char *value="Now this is an array";

    ret=cso_set_str(obj,field,1,value);

    if(ret!=0) {
      printf("Error while setting string field \"%s\": %d...aborting!\n",
        field,ret);
      cso_delete(obj);
      return 1;
    } else {
      printf("Success, field: \"%s\", value: \"%s\"\n",
        field,value);

      int arrSize=cso_get_arrsize(obj,field);
      printf("Arraysize of field \"%s\" is now: %d\n",field,arrSize);

      const char *ptr=cso_get_str(obj,field,0);

      if(ptr) {
        printf("Retrieved value of field \"%s\" in index 0: \"%s\"\n",field,ptr);
      } else {
        printf("Couldn't retrieve value of field \"%s\"... aborting!\n",field);
        cso_delete(obj);
        return 1;
      }

      ptr=cso_get_str(obj,field,1);

      if(ptr) {
        printf("Retrieved value of field \"%s\" in index 1: \"%s\"\n",field,ptr);
      } else {
        printf("Couldn't retrieve value of field \"%s\"... aborting!\n",field);
        cso_delete(obj);
        return 1;
      }
    }

    printf("\n");
  }

  {
    const char *field="int_2";
    printf("Forcing field \"%s\" to be an array...\n",field);
    ret=cso_force_array(obj,field);

    if(ret!=0) {
      printf("Error while forcing array on field field \"%s\": %d\n",
        field,ret);
      cso_delete(obj);
      return 1;
    } else {
      printf("Success\n");

      int arrSize=cso_get_arrsize(obj,field);
      printf("Arraysize of field \"%s\" is now: %d\n",field,arrSize);

      int val=cso_get_int(obj,field,0);

      if(val!=0) {
        printf("Retrieved (int) value of field \"%s\" in index 0: %d\n",field,val);
      } else {
        printf("Couldn't retrieve (int) value of field \"%s\"... aborting!\n",field);
        cso_delete(obj);
        return 1;
      }
    }

    {
      const char *field="non-existent";
      int arrSize=cso_get_arrsize(obj,field);
      printf("Arraysize of non-existent field is: %d\n",arrSize);
    }

    printf("\n");
  }

  /**
   * serialize
   */
  if(serialize(obj,&buf,&buflen)) {
    cso_delete(obj);
    return 1;
  }

  /**
   * big array (with reallocation)
   */
  {
    printf("Creating big array...\n");

    const char *field="big_array";
    char value[9];
    int i;

    for(i=0; i<256; i++) {
      ret=snprintf(value,9,"value%d",i);

      if(ret>0) {
        ret=cso_set_str(obj,field,i,value);

        if(ret!=0) {
          printf("Error while setting string field \"%s\" at index %d: %d...aborting!\n",
            field,i,ret);
          cso_delete(obj);
          return 1;
        }
      } else {
        printf("Couldn't snprintf current value for index %d... aborting!\n",i);
        cso_delete(obj);
        return 1;
      }
    }

    printf("\n");
  }

  /**
   * serialize
   */
  if(serialize(obj,&buf,&buflen)) {
    cso_delete(obj);
    return 1;
  }

  /**
   * deserialize
   */
  printf("Deserialize json data...\n");

  const char *json="// Einleitender Kommentar\n\n"
                    "{"
                    "  \"int\" : 12,"
                    "  \"string\": \"Ein Teststring\", // Kommentar dahinter\n"
                    "  \"sub_obj\" : {"
                    "    \"sub_string\" : \"ein weiterer Test\","
                    "    \"sub_arr\":[ 12 , 34 , 56 , 78 , 90 ]"
                    "  },"
                    "  \"array\": [\"arr_1\", \"arr_2\", \"arr_3\"]"
                    "}";
  container *deserCont=cso_new("deserialized");
  ret=cso_deserialize(deserCont,json,strlen(json));

  printf("Bytes deserialized: %d (of %d)\n",ret,(int)strlen(json));

  if(serialize(deserCont,&buf,&buflen)) {
    cso_delete(deserCont);
    cso_delete(obj);
    return 1;
  }

  cso_delete(deserCont);

  /**
   * big binary data, copy this big object and compare
   */

  printf("Creating a big binary field, serialize, deserialize and compare it \n");

  {
    const char *field="bin_1";
    char *value=NULL;
    int len=20000;
    int i=-1;

    value=(char *)malloc(len);
    while (++i<len)
      value[i]=random()%256;

    ret=cso_set_bin(obj,field,value,len,0);

    if(ret!=0) {
      printf("Error while setting big binary field \"%s\": %d\n",
        field,ret);
      cso_delete(obj);
      return 1;
    } else {
      printf("Successfully set binary field: \"%s\", len: %d\n",
        field,len);

      char *serData=NULL;
      int serLen=0;

      ret=cso_serialize(obj,&serData,&serLen);

      if(ret==0) {
        printf("Serialized object with binary field, len: %d\n",serLen);
      } else {
        printf("Couldn't serialize object with binary field... aborting!\n");
        cso_delete(obj);
        return 1;
      }

      int strLen=strlen(serData);

      if(strLen<serLen) {
        printf("Serialized data is partially binary, string length of %d is less than serialized length of %d\n",
          strLen,serLen);
      }

      printf("Delete obj and create a new one\n");
      cso_delete(obj);
      obj=cso_new("obj");

      printf("Deserialize previously serialized object... ");
      ret=cso_deserialize(obj,serData,serLen);

      if(ret!=-1) {
        printf("success, deserialized %d bytes of data\n",ret);

        char *binData=NULL;
        int binLen=0;
        
        ret=cso_get_bin(obj,field,0,&binData,&binLen);

        if(ret==0) {
          printf("Retrieved value of big binary field \"%s\" within deserialization with len %d\n",field,binLen);

          int lenIsEqual=(binLen==len)?1:0;

          if(lenIsEqual) {
            printf("Success, binary data field length in source and deserialization is equal\n");
          } else {
            printf("Failed, binary data field length in source and deserialization is NOT equal... aborting!\n");
            cso_delete(obj);
            return 1;
          }
        } else {
          printf("Couldn't retrieve value of field \"%s\" within deserialization... aborting!\n",field);
          cso_delete(obj);
          return 1;
        }
      } else {
        printf("failed\n"
            "Couldn't deserialize object with binary data ... aborting!\n");
        cso_delete(obj);
        return 1;
      }
    }

    printf("\n");
  }

  /*
   *  exit
   */
  cso_delete(obj);  
  return 0;
}
