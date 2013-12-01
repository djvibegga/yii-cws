<div>Hello I am a first view file!</div>

<div>admin panel URL is: {$adminURL}</div>

<div>Parameters:</div>
<table>
    <tr>
        <td>Key</td>
        <td>Value</td>
    </tr>
    {% for param in params %}
    <tr>
        <td>
        {$param.key}
        </td>
        <td>
        {$param.value}
        </td>
    </tr>
    {% endfor %}
</table>