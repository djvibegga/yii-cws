<div>
    <span><a href="{$adminURL}">Visit admin URL</a></span>
</div>

<table>
    <tr>
        <td>ID</td>
        <td>Name</td>
    </tr>
    {% for goal in goals %}
    <tr>
        <td>{$goal.id}</td>
        <td>{$goal.name}</td>
    </tr>
    {% endfor %}
</table>