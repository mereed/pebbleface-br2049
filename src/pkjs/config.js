module.exports = [
  {
    "type": "heading",
    "defaultValue": "Configuration"
  },
     {
        "type": "text",
        "defaultValue": "<h6>A White button = OFF.  An Orange button = ON</h6>",
      },	

	{
    "type": "section",
    "items": [

		{
  "type": "select",
  "messageKey": "digtime",
  "defaultValue": 0,
  "label": "Date format",
  "options": [


    { 
      "label": "US Date",
      "value": 1
    },
	      { 
      "label": "INT Date",
      "value": 2
    }
  ]
},
	]
	},
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];